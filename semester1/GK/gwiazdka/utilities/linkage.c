
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "openglheader.h"
#include <GL/glu.h>

#include "utilities.h"
#include "linkage.h"

/* ///////////////////////////////////////////////////////////////////////// */
kl_linkage *kl_NewLinkage ( int maxo, int maxl, int maxr,
                            int maxj, int maxp, void *usrdata )
{
  kl_linkage *lkg;

  lkg = malloc ( sizeof(kl_linkage) );
  if ( lkg ) {
    memset ( lkg, 0, sizeof(kl_linkage) );
    lkg->maxobj    = maxo;
    lkg->maxlinks  = maxl;
    lkg->maxorefs  = maxr;
    lkg->maxhj     = 2*maxj;
    lkg->maxartpar = maxp;
    lkg->obj  = malloc ( maxo*sizeof(kl_object) );
    lkg->link = malloc ( maxl*sizeof(kl_link) );
    lkg->oref = malloc ( maxr*sizeof(kl_obj_ref) );
    lkg->hj   = malloc ( 2*maxj*sizeof(kl_halfjoint) );
    lkg->artp = malloc ( 2*maxp*sizeof(GLfloat) );
    if ( !lkg->obj || !lkg->link || !lkg->oref || !lkg->hj || !lkg->artp )
      goto failure;
    lkg->prevartp = &lkg->artp[maxp];
    memset ( lkg->artp, 0, maxp*sizeof(GLfloat) );
    memset ( lkg->prevartp, 0x7f, maxp*sizeof(GLfloat) );
    lkg->current_root = 0;
    M4x4Identf ( lkg->current_root_tr );
    lkg->usrdata = usrdata;
  }
  return lkg;

failure:
  if ( lkg->obj )  free ( lkg->obj );
  if ( lkg->link ) free ( lkg->link );
  if ( lkg->oref ) free ( lkg->oref );
  if ( lkg->hj )   free ( lkg->hj );
  if ( lkg->artp ) free ( lkg->artp );
  free ( lkg );
  return NULL;
} /*kl_NewLinkage*/

void kl_DestroyLinkage ( kl_linkage *linkage )
{
  int i;

  if ( linkage->obj ) {
    for ( i = 0; i < linkage->nobj; i++ ) {
      if ( linkage->obj[i].destroy )
        linkage->obj[i].destroy ( &linkage->obj[i], linkage->usrdata );
    }
    free ( linkage->obj );
  }
  if ( linkage->oref ) {
    for ( i = 0; i < linkage->norefs; i++ )
      if ( linkage->oref[i].vn )
        free ( linkage->oref[i].vn );
    free ( linkage->oref );
  }
  if ( linkage->link ) free ( linkage->link );
  if ( linkage->hj )   free ( linkage->hj );
  if ( linkage->artp ) free ( linkage->artp );
  free ( linkage );
} /*kl_DestroyLinkage*/

/* ///////////////////////////////////////////////////////////////////////// */
void kl_DefaultTransform ( kl_object *obj, int refn, GLfloat *tr,
                           int nv, int *vn, void *lkgusrdata )
{
  int     i, k;
  GLfloat *vert, *tvert;

  if ( !(vert = obj->vert) || !(tvert = obj->tvert) )
    return;
  if ( vn ) {
    switch ( obj->nvc ) {
  case 3:
      for ( i = 0; i < nv; i++ ) {
        k = 3*vn[i];
        M4x4MultMP3f ( &tvert[k], tr, &vert[k] );
      }
      break;
  case 4:
      for ( i = 0; i < nv; i++ ) {
        k = 4*vn[i];
        M4x4MultMVf ( &tvert[k], tr, &vert[k] );
      }
      break;
  default:
      return;
    }
  }
  else {
    switch ( obj->nvc ) {
  case 3:
      for ( i = k = 0;  i < nv;  i++, k += 3 )
        M4x4MultMP3f ( &tvert[k], tr, &vert[k] );
      break;
  case 4:
      for ( i = k = 0;  i < nv;  i++, k += 4 )
        M4x4MultMVf ( &tvert[k], tr, &vert[k] );
      break;
  default:
      return;
    }
  }
} /*kl_DefaultTransform*/

void kl_obj_stub ( kl_object *obj, void *lkgusrdata )
{
} /*kl_obj_stub*/

int kl_NewObject ( kl_linkage *linkage, int nvc, int nvert,
                   const GLfloat *etrans, void *usrdata,
                   kl_obj_init init,
                   kl_obj_transform transform,
                   kl_obj_postprocess postprocess,
                   kl_obj_redraw redraw,
                   kl_obj_destroy destroy )
{
  int         on;
  kl_object *obj;

  if ( linkage->nobj < linkage->maxobj ) {
    on = linkage->nobj;
    obj = &linkage->obj[on];
    memset ( obj, 0, sizeof(kl_object) );
    if ( etrans )
      memcpy ( obj->Etr, etrans, 16*sizeof(GLfloat) );
    else
      M4x4Identf ( obj->Etr );
    obj->transform = transform != NULL ? transform : kl_DefaultTransform;
    obj->postprocess = postprocess != NULL ? postprocess : kl_obj_stub;
    obj->redraw = redraw != NULL ? redraw : kl_obj_stub;
    obj->destroy = destroy != NULL ? destroy : kl_obj_stub;
    obj->nvc = nvc;
    obj->nvert = nvert;
    obj->vert = obj->tvert = NULL;
    obj->usrdata = usrdata;
    if ( !init || init ( obj, usrdata, linkage->usrdata ) ) {
      linkage->nobj ++;
      return on;
    }
  }
  return -1;
} /*kl_NewObject*/

int kl_NewLink ( kl_linkage *linkage )
{
  int     lkn;
  kl_link *lk;

  if ( linkage->nlinks < linkage->maxlinks ) {
    lkn = linkage->nlinks ++;
    lk = &linkage->link[lkn];
    memset ( lk, 0, sizeof(kl_link) );
    lk->fref = lk->fhj = -1;
    lk->tag = 0;
    return lkn;
  }
  return -1;
} /*kl_NewLink*/

int kl_NewObjRef ( kl_linkage *linkage, int lkn, int on, int nv, int *vn )
{
  int        orn;
  kl_obj_ref *oref;

  if ( linkage->norefs < linkage->maxorefs ) {
    orn = linkage->norefs ++;
    oref = &linkage->oref[orn];
    memset ( oref, 0, sizeof(kl_obj_ref) );
    oref->on = on;
    oref->nv = nv;
    if ( vn ) {
      if ( !(oref->vn = malloc ( nv*sizeof(int) )) )
        return -1;
      memcpy ( oref->vn, vn, nv*sizeof(int) );
    }
    else
      oref->vn = NULL;
    oref->nextr = linkage->link[lkn].fref;
    linkage->link[lkn].fref = orn;
    return orn;
  }
  return -1;
} /*kl_NewObjRef*/

int kl_NewJoint ( kl_linkage *linkage, int l0, int l1, int art, int pnum )
{
  int          jn0, jn1;
  kl_halfjoint *hj0, *hj1;

  if ( linkage->nhj < linkage->maxhj-1 &&
       l0 < linkage->nlinks && l1 < linkage->nlinks ) {
    jn0 = linkage->nhj ++;
    jn1 = linkage->nhj ++;
    hj0 = &linkage->hj[jn0];
    hj1 = &linkage->hj[jn1];
    memset ( hj0, 0, 2*sizeof(kl_halfjoint) );
    hj0->otherhalf = jn1;
    hj1->otherhalf = jn0;
    hj0->l0 = hj1->l1 = l0;
    hj0->l1 = hj1->l0 = l1;
    hj0->art = art;
    hj0->pnum = pnum;
    hj1->art = art == KL_ART_NONE ? KL_ART_NONE : KL_ART_OTHERHALF;
    hj1->pnum = -1;
    hj0->nexthj = linkage->link[l0].fhj;
    linkage->link[l0].fhj = jn0;
    hj1->nexthj = linkage->link[l1].fhj;
    linkage->link[l1].fhj = jn1;
    M4x4Identf ( hj0->Ftr );
    M4x4Identf ( hj0->Rtr );
    M4x4Identf ( hj0->Btr );
    M4x4Identf ( hj1->Ftr );
    M4x4Identf ( hj1->Rtr );
    M4x4Identf ( hj1->Btr );
    return jn0;
  }
  return -1;
} /*kl_NewJoint*/

/* ///////////////////////////////////////////////////////////////////////// */
void kl_SetJointFtr ( kl_linkage *linkage, int jn, GLfloat *tr, char back )
{
  kl_halfjoint *hj, *hj1;

  if ( jn < 0 || jn >= linkage->nhj )
    return;
  hj = &linkage->hj[jn];
  hj1 = &linkage->hj[hj->otherhalf];
  memcpy ( &hj->Ftr, tr, 16*sizeof(GLfloat) );
  M4x4Invertf ( hj1->Btr, tr );
  if ( back ) {
    memcpy ( hj->Btr, hj1->Btr, 16*sizeof(GLfloat) );
    memcpy ( hj1->Ftr, hj->Ftr, 16*sizeof(GLfloat) );
  }
} /*kl_SetJointFtr*/

void kl_SetJointBtr ( kl_linkage *linkage, int jn, GLfloat *tr, char front )
{
  kl_halfjoint *hj, *hj1;

  if ( jn < 0 || jn >= linkage->nhj )
    return;
  hj = &linkage->hj[jn];
  hj1 = &linkage->hj[hj->otherhalf];
  memcpy ( &hj->Btr, tr, 16*sizeof(GLfloat) );
  M4x4Invertf ( hj1->Ftr, tr );
  if ( front ) {
    memcpy ( hj->Ftr, hj1->Ftr, 16*sizeof(GLfloat) );
    memcpy ( hj1->Btr, hj->Btr, 16*sizeof(GLfloat) );
  }
} /*kl_SetJointBtr*/

/* ///////////////////////////////////////////////////////////////////////// */
void kl_SetArtParam ( kl_linkage *linkage, int pno, int nump, GLfloat *par )
{
  if ( nump > 0 && pno >= 0 && pno+nump <= linkage->maxartpar ) {
    memcpy ( &linkage->artp[pno], par, nump*sizeof(GLfloat) );
    if ( pno+nump > linkage->nartpar )
      linkage->nartpar = pno+nump;
  }
} /*kl_SetArtParam*/

static char _kl_changed ( GLfloat *a, GLfloat *b, int i, int n )
{
  return memcmp ( &a[i], &b[i], n*sizeof(GLfloat) ) != 0;
} /*_kl_changed*/

static void _kl_UpdateArtTr ( kl_linkage *linkage, int jn )
{
  kl_halfjoint *hj, *hj1;
  int          pnum;
  GLfloat      *artp, *prevartp;

  artp = linkage->artp;
  prevartp = linkage->prevartp;
  hj = &linkage->hj[jn];
  if ( hj->art == KL_ART_OTHERHALF )
    return;
  hj1 = &linkage->hj[hj->otherhalf];
  pnum = hj->pnum;
  switch ( hj->art ) {
case KL_ART_NONE:
    break;

case KL_ART_TRANS_X:
    if ( _kl_changed ( artp, prevartp, pnum, 1 ) ) {
      M4x4Translatef ( hj->Rtr, artp[pnum], 0.0, 0.0 );
      M4x4Translatef ( hj1->Rtr, -artp[pnum], 0.0, 0.0 );
    }
    break;

case KL_ART_TRANS_Y:
    if ( _kl_changed ( artp, prevartp, pnum, 1 ) ) {
      M4x4Translatef ( hj->Rtr, 0.0, artp[pnum], 0.0 );
      M4x4Translatef ( hj1->Rtr, 0.0, -artp[pnum], 0.0 );
    }
    break;

case KL_ART_TRANS_Z:
    if ( _kl_changed ( artp, prevartp, pnum, 1 ) ) {
      M4x4Translatef ( hj->Rtr, 0.0, 0.0, artp[pnum] );
      M4x4Translatef ( hj1->Rtr, 0.0, 0.0, -artp[pnum] );
    }
    break;

case KL_ART_TRANS_XYZ:
    if ( _kl_changed ( artp, prevartp, pnum, 3 ) ) {
      M4x4Translatef ( hj->Rtr, artp[pnum], artp[pnum+1], artp[pnum+2] );
      M4x4Translatef ( hj1->Rtr, -artp[pnum], -artp[pnum+1], -artp[pnum+2] );
    }
    break;

case KL_ART_SCALE_X:
    if ( _kl_changed ( artp, prevartp, pnum, 1 ) ) {
      M4x4Scalef ( hj->Rtr, artp[pnum], 1.0, 1.0 );
      M4x4Scalef ( hj1->Rtr, 1.0/artp[pnum],1.0, 1.0 );
    }
    break;

case KL_ART_SCALE_Y:
    if ( _kl_changed ( artp, prevartp, pnum, 1 ) ) {
      M4x4Scalef ( hj->Rtr, 1.0, artp[pnum], 1.0 );
      M4x4Scalef ( hj1->Rtr, 1.0, 1.0/artp[pnum],1.0 );
    }
    break;

case KL_ART_SCALE_Z:
    if ( _kl_changed ( artp, prevartp, pnum, 1 ) ) {
      M4x4Scalef ( hj->Rtr, 1.0, 1.0, artp[pnum] );
      M4x4Scalef ( hj1->Rtr, 1.0, 1.0, 1.0/artp[pnum] );
    }
    break;

case KL_ART_SCALE_XYZ:
    if ( _kl_changed ( artp, prevartp, pnum, 3 ) ) {
      M4x4Scalef ( hj->Rtr, artp[pnum], artp[pnum+1], artp[pnum+2] );
      M4x4Scalef ( hj1->Rtr, 1.0/artp[pnum], 1.0/artp[pnum+1], 1.0/artp[pnum+2] );
    }
    break;

case KL_ART_ROT_X:
    if ( _kl_changed ( artp, prevartp, pnum, 1 ) ) {
      M4x4RotateXf ( hj->Rtr, artp[pnum] );
      M4x4RotateXf ( hj1->Rtr, -artp[pnum] );
    }
    break;

case KL_ART_ROT_Y:
    if ( _kl_changed ( artp, prevartp, pnum, 1 ) ) {
      M4x4RotateYf ( hj->Rtr, artp[pnum] );
      M4x4RotateYf ( hj1->Rtr, -artp[pnum] );
    }
    break;

case KL_ART_ROT_Z:
    if ( _kl_changed ( artp, prevartp, pnum, 1 ) ) {
      M4x4RotateZf ( hj->Rtr, artp[pnum] );
      M4x4RotateZf ( hj1->Rtr, -artp[pnum] );
    }
    break;

case KL_ART_ROT_V:
    if ( _kl_changed ( artp, prevartp, pnum, 4 ) ) {
      M4x4RotateVf ( hj->Rtr, artp[pnum], artp[pnum+1], artp[pnum+2],
                     artp[pnum+3] );
      M4x4Transposef ( hj1->Rtr, hj->Rtr );
    }
    break;

default:
    M4x4Identf ( hj->Rtr );
    M4x4Identf ( hj1->Rtr );
    break;
  }
} /*_kl_UpdateArtTr*/

static void _kl_rArticulate ( kl_linkage *linkage, int lkn, GLfloat *tr )
{
  kl_link      *lk;
  GLfloat      t0[16], t1[16];
  int          r, on, j, l1;
  kl_object    *obj;
  kl_obj_ref   *oref;
  kl_halfjoint *hj;

  obj = linkage->obj;
  oref = linkage->oref;
  hj = linkage->hj;
  lk = &linkage->link[lkn];
  lk->tag = 1;
        /* pass through the list of object references and transform */
        /* the object vertices */
  for ( r = lk->fref;  r >= 0;  r = oref[r].nextr ) {
    on = oref[r].on;
    M4x4Multf ( t1, tr, obj[on].Etr );
    obj[on].transform ( &obj[on], r, t1, oref[r].nv, oref[r].vn, linkage->usrdata );
  }
        /* now traverse the list of halfjoints originating at */
        /* the current link to (DFS) search the linkage graph */
  for ( j = lk->fhj; j >= 0; j = hj[j].nexthj ) {
    l1 = hj[j].l1;
    if ( !linkage->link[l1].tag ) {
      M4x4Multf ( t0, tr, hj[j].Ftr );
      M4x4Multf ( t1, t0, hj[j].Rtr );
      M4x4Multf ( t0, t1, hj[j].Btr );
      _kl_rArticulate ( linkage, l1, t0 );
    }
  }
} /*_kl_rArticulate*/

void kl_Articulate ( kl_linkage *linkage )
{
  int       i;

  for ( i = 0; i < linkage->nlinks; i++ )
    linkage->link[i].tag = 0;
  for ( i = 0; i < linkage->nhj; i++ )
    _kl_UpdateArtTr ( linkage, i );
  _kl_rArticulate ( linkage, linkage->current_root, linkage->current_root_tr );
  for ( i = 0; i < linkage->nobj; i++ )
    linkage->obj[i].postprocess ( &linkage->obj[i], linkage->usrdata );
  memcpy ( linkage->prevartp, linkage->artp, linkage->maxartpar*sizeof(GLfloat) );
} /*kl_Articulate*/

void kl_Redraw ( kl_linkage *linkage )
{
  int i;

  for ( i = 0; i < linkage->nobj; i++ )
    linkage->obj[i].redraw ( &linkage->obj[i], linkage->usrdata );
} /*kl_Redraw*/

