"""Practical 1

Greatly inspired by Stanford CS224 2019 class.
"""

import pprint

import matplotlib.pyplot as plt
from gensim.models import KeyedVectors
import nltk

nltk.download('reuters')
nltk.download('pl196x')
import random

import numpy as np
from nltk.corpus.reader import pl196x
from sklearn.decomposition import TruncatedSVD

import itertools
import requests
import zipfile
import io
import os


START_TOKEN = '<START>'
END_TOKEN = '<END>'

np.random.seed(0)
random.seed(0)


#################################
# TODO: a)
def distinct_words(corpus):
    """ Determine a list of distinct words for the corpus.
        Params:
            corpus (list of list of strings): corpus of documents
        Return:
            corpus_words (list of strings): list of distinct words across the
            corpus, sorted (using python 'sorted' function)
            num_corpus_words (integer): number of distinct words across the
            corpus
    """
    corpus_words = []
    num_corpus_words = -1

    # ------------------
    # Write your implementation here.
    words = list(itertools.chain(*corpus))
    corpus_words = sorted(list(set(words)))
    num_corpus_words = len(corpus_words)

    # ------------------

    return corpus_words, num_corpus_words


# ---------------------
# Run this sanity check
# Note that this not an exhaustive check for correctness.
# ---------------------

# Define toy corpus
test_corpus = ["START Ala miec kot i pies END".split(" "),
               "START Ala lubic kot END".split(" ")]
test_corpus_words, num_corpus_words = distinct_words(test_corpus)

# Correct answers
ans_test_corpus_words = sorted(list({'Ala', 'END', 'START', 'i', 'kot', 'lubic', 'miec', 'pies'}))
ans_num_corpus_words = len(ans_test_corpus_words)

# Test correct number of words
assert (num_corpus_words == ans_num_corpus_words), "Incorrect number of distinct words. Correct: {}. Yours: {}".format(
    ans_num_corpus_words, num_corpus_words)

# Test correct words
assert (test_corpus_words == ans_test_corpus_words), "Incorrect corpus_words.\nCorrect: {}\nYours:   {}".format(
    str(ans_test_corpus_words), str(test_corpus_words))

# Print Success
print("-" * 80)
print("Passed All Tests!")
print("-" * 80)


#################################
# TODO: b)
def compute_co_occurrence_matrix(corpus, window_size=4):
    """ Compute co-occurrence matrix for the given corpus and window_size (default of 4).

        Note: Each word in a document should be at the center of a window.
            Words near edges will have a smaller number of co-occurring words.

              For example, if we take the document "START All that glitters is not gold END" with window size of 4,
              "All" will co-occur with "START", "that", "glitters", "is", and "not".

        Params:
            corpus (list of list of strings): corpus of documents
            window_size (int): size of context window
        Return:
            M (numpy matrix of shape (number of corpus words, number of corpus words)):
                Co-occurence matrix of word counts.
                The ordering of the words in the rows/columns should be the
                same as the ordering of the words given by the distinct_words
                function.
            word2Ind (dict): dictionary that maps word to index
                (i.e. row/column number) for matrix M.
    """
    words, num_words = distinct_words(corpus)
    M = None
    word2Ind = {}

    # ------------------
    # Write your implementation here.
    word2Ind = dict(zip(words, range(num_words)))
    M = np.zeros((num_words, num_words))

    for sentence in corpus:
        for i, word1 in enumerate(sentence):
            for word2 in sentence[max(i-window_size, 0): min(i+window_size+1, len(sentence))]:
                if word1 != word2:
                    M[word2Ind[word1], word2Ind[word2]] += 1
    # ------------------

    return M, word2Ind


# ---------------------
# Run this sanity check
# Note that this is not an exhaustive check for correctness.
# ---------------------

# Define toy corpus and get student's co-occurrence matrix
test_corpus = ["START Ala miec kot i pies END".split(" "),
               "START Ala lubic kot END".split(" ")]
M_test, word2Ind_test = compute_co_occurrence_matrix(
    test_corpus, window_size=1)

# Correct M and word2Ind
M_test_ans = np.array([
    [0., 0., 2., 0., 0., 1., 1., 0.],
    [0., 0., 0., 0., 1., 0., 0., 1.],
    [2., 0., 0., 0., 0., 0., 0., 0.],
    [0., 0., 0., 0., 1., 0., 0., 1.],
    [0., 1., 0., 1., 0., 1., 1., 0.],
    [1., 0., 0., 0., 1., 0., 0., 0.],
    [1., 0., 0., 0., 1., 0., 0., 0.],
    [0., 1., 0., 1., 0., 0., 0., 0.]
])

word2Ind_ans = {
    'Ala': 0, 'END': 1, 'START': 2, 'i': 3, 'kot': 4, 'lubic': 5, 'miec': 6,
    'pies': 7}

# Test correct word2Ind
assert (word2Ind_ans == word2Ind_test), "Your word2Ind is incorrect:\nCorrect: {}\nYours: {}".format(word2Ind_ans,
                                                                                                     word2Ind_test)

# Test correct M shape
assert (M_test.shape == M_test_ans.shape), "M matrix has incorrect shape.\nCorrect: {}\nYours: {}".format(M_test.shape,
                                                                                                          M_test_ans.shape)

# Test correct M values
for w1 in word2Ind_ans.keys():
    idx1 = word2Ind_ans[w1]
    for w2 in word2Ind_ans.keys():
        idx2 = word2Ind_ans[w2]
        student = M_test[idx1, idx2]
        correct = M_test_ans[idx1, idx2]
        if student != correct:
            print("Correct M:")
            print(M_test_ans)
            print("Your M: ")
            print(M_test)
            raise AssertionError(
                "Incorrect count at index ({}, {})=({}, {}) in matrix M. Yours has {} but should have {}.".format(idx1,
                                                                                                                  idx2,
                                                                                                                  w1,
                                                                                                                  w2,
                                                                                                                  student,
                                                                                                                  correct))

# Print Success
print("-" * 80)
print("Passed All Tests!")
print("-" * 80)


#################################
# TODO: c)
def reduce_to_k_dim(M, k=2):
    """ Reduce a co-occurence count matrix of dimensionality
        (num_corpus_words, num_corpus_words)
        to a matrix of dimensionality (num_corpus_words, k) using the following
         SVD function from Scikit-Learn:
            - http://scikit-learn.org/stable/modules/generated/sklearn.decomposition.TruncatedSVD.html

        Params:
            M (numpy matrix of shape (number of corpus words, number
                of corpus words)): co-occurence matrix of word counts
            k (int): embedding size of each word after dimension reduction
        Return:
            M_reduced (numpy matrix of shape (number of corpus words, k)):
            matrix of k-dimensioal word embeddings.
            In terms of the SVD from math class, this actually returns U * S
    """
    n_iters = 10  # Use this parameter in your call to `TruncatedSVD`
    M_reduced = None
    print("Running Truncated SVD over %i words..." % (M.shape[0]))

    # ------------------
    # Write your implementation here.
    svd = TruncatedSVD(n_components=k, n_iter=n_iters)
    M_reduced = svd.fit_transform(M)
    # ------------------

    print("Done.")
    return M_reduced


# ---------------------
# Run this sanity check
# Note that this not an exhaustive check for correctness
# In fact we only check that your M_reduced has the right dimensions.
# ---------------------

# Define toy corpus and run student code
test_corpus = ["START Ala miec kot i pies END".split(" "),
               "START Ala lubic kot END".split(" ")]
M_test, word2Ind_test = compute_co_occurrence_matrix(test_corpus, window_size=1)
M_test_reduced = reduce_to_k_dim(M_test, k=2)

# Test proper dimensions
assert (M_test_reduced.shape[0] == 8), "M_reduced has {} rows; should have {}".format(M_test_reduced.shape[0], 8)
assert (M_test_reduced.shape[1] == 2), "M_reduced has {} columns; should have {}".format(M_test_reduced.shape[1], 2)

# Print Success
print("-" * 80)
print("Passed All Tests!")
print("-" * 80)


#################################
# TODO: d)
def plot_embeddings(M_reduced, word2Ind, words, save=False, filename=None):
    """ Plot in a scatterplot the embeddings of the words specified
        in the list "words".
        NOTE: do not plot all the words listed in M_reduced / word2Ind.
        Include a label next to each point.

        Params:
            M_reduced (numpy matrix of shape (number of unique words in the
            corpus , k)): matrix of k-dimensioal word embeddings
            word2Ind (dict): dictionary that maps word to indices for matrix M
            words (list of strings): words whose embeddings we want to
            visualize
    """

    # ------------------
    # Write your implementation here.
    x = [M_reduced[word2Ind[word], 0] for word in words]
    y = [M_reduced[word2Ind[word], 1] for word in words]

    fig, ax = plt.subplots()
    ax.scatter(x, y)

    for i, word in enumerate(words):
        ax.annotate(word, (x[i], y[i]))

    if save:
        plt.savefig(f'{filename}.png')
    else:
        fig.show()
    # ------------------#


# ---------------------
# Run this sanity check
# Note that this not an exhaustive check for correctness.
# The plot produced should look like the "test solution plot" depicted below.
# ---------------------

print("-" * 80)
print("Outputted Plot:")

M_reduced_plot_test = np.array([[1, 1], [-1, -1], [1, -1], [-1, 1], [0, 0]])
word2Ind_plot_test = {
    'test1': 0, 'test2': 1, 'test3': 2, 'test4': 3, 'test5': 4}
words = ['test1', 'test2', 'test3', 'test4', 'test5']
plot_embeddings(M_reduced_plot_test, word2Ind_plot_test, words, True, 'test_plot')

print("-" * 80)


#################################
# TODO: e)
# -----------------------------
# Run This Cell to Produce Your Plot
# ------------------------------

def read_corpus_pl():
    """ Read files from the specified Reuter's category.
        Params:
            category (string): category name
        Return:
            list of lists, with words from each of the processed files
    """
    pl196x_dir = nltk.data.find('corpora/pl196x')
    pl = pl196x.Pl196xCorpusReader(
        pl196x_dir, r'.*\.xml', textids='textids.txt', cat_file="cats.txt")
    tsents = pl.tagged_sents(fileids=pl.fileids(), categories='cats.txt')[:5000]

    return [[START_TOKEN] + [
        w[0].lower() for w in list(sent)] + [END_TOKEN] for sent in tsents]


def plot_unnormalized(corpus, words):
    M_co_occurrence, word2Ind_co_occurrence = compute_co_occurrence_matrix(
        corpus)
    M_reduced_co_occurrence = reduce_to_k_dim(M_co_occurrence, k=2)
    plot_embeddings(M_reduced_co_occurrence, word2Ind_co_occurrence, words, True, 'unnormalized_plot')


def plot_normalized(corpus, words):
    M_co_occurrence, word2Ind_co_occurrence = compute_co_occurrence_matrix(
        corpus)
    M_reduced_co_occurrence = reduce_to_k_dim(M_co_occurrence, k=2)
    # Rescale (normalize) the rows to make them each of unit-length
    M_lengths = np.linalg.norm(M_reduced_co_occurrence, axis=1)
    M_normalized = M_reduced_co_occurrence / M_lengths[:, np.newaxis]  # broadcasting
    plot_embeddings(M_normalized, word2Ind_co_occurrence, words, True, 'normalized_plot')


pl_corpus = read_corpus_pl()
words = [
    "sztuka", "śpiewaczka", "literatura", "poeta", "obywatel"]

plot_normalized(pl_corpus, words)
plot_unnormalized(pl_corpus, words)

# Answers:
#
# What clusters together in 2-dimensional embedding space?
# In the first plot, normalized, we have two clusters. First with "sztuka"
# and second with "śpiewaczka", "poeta", "obywatel", "literatura".
# In the second plot, unnormalized, we have three clusters. First with "sztuka",
# second with "literatura" and third with "śpiewaczka", "poeta", "obywatel".
#
# What doesn’t cluster together that you might think should have?
# "sztuka" and "poeta" intuitively should be close to each other but aren't on both plots.
#
# Is normalization necessary?
# In my opinion unnormalized plot is better, because it's more intuitive when both "literatura"
# and "sztuka" are both far apart from people.

#################################
# Section 2:
#################################
# Then run the following to load the word2vec vectors into memory.
# Note: This might take several minutes.

if not (os.path.isfile('word2vec_100_3_polish.bin') and os.path.isfile('word2vec_100_3_polish.bin.syn0.npy')):
    url = 'https://github.com/sdadas/polish-nlp-resources/releases/download/v1.0/word2vec.zip'
    req = requests.get(url)
    zip_file = zipfile.ZipFile(io.BytesIO(req.content))
    zip_file.extractall()

wv_from_bin_pl = KeyedVectors.load("word2vec_100_3_polish.bin")


# -----------------------------------
# Run Cell to Load Word Vectors
# Note: This may take several minutes
# -----------------------------------


#################################
# TODO: a)
def get_matrix_of_vectors(wv_from_bin, required_words):
    """ Put the word2vec vectors into a matrix M.
        Param:
            wv_from_bin: KeyedVectors object; the 3 million word2vec vectors
                         loaded from file
        Return:
            M: numpy matrix shape (num words, 300) containing the vectors
            word2Ind: dictionary mapping each word to its row number in M
    """
    words = list(wv_from_bin.key_to_index.keys())
    print("Shuffling words ...")
    random.shuffle(words)
    words = words[:10000]
    print("Putting %i words into word2Ind and matrix M..." % len(words))
    word2Ind = {}
    M = []
    curInd = 0
    for w in words:
        try:
            M.append(wv_from_bin.word_vec(w))
            word2Ind[w] = curInd
            curInd += 1
        except KeyError:
            continue
    for w in required_words:
        try:
            M.append(wv_from_bin.word_vec(w))
            word2Ind[w] = curInd
            curInd += 1
        except KeyError:
            continue
    M = np.stack(M)
    print("Done.")
    return M, word2Ind


# -----------------------------------------------------------------
# Run Cell to Reduce 300-Dimensinal Word Embeddings to k Dimensions
# Note: This may take several minutes
# -----------------------------------------------------------------

#################################
# TODO: a)
M, word2Ind = get_matrix_of_vectors(wv_from_bin_pl, words)
M_reduced = reduce_to_k_dim(M, k=2)

words = [
    "sztuka", "śpiewaczka", "literatura", "poeta", "obywatel"]
plot_embeddings(M_reduced, word2Ind, words, True, 'reduced_plot')

# Answers:
#
# What clusters together in 2-dimensional embedding space?
# I don't see so close words as in earlier plots. The closer ones are
# "obywatel" and "literatura". "śpiewaczka" is furthest away from the rest of the words.
#
# What doesn’t cluster together that you might think should have?
# I'm a little suprised that "literatura" is closer to "obywatel" than to "poeta".
#
# How is the plot different from the one generated earlier from the co-occurrence matrix?
# Words are futher away from each others. "sztuka" and "literatura" are now reasonably close
# comparing to the whole plot.
#

#################################
# TODO: b)
# Polysemous Words
# ------------------
# Write your polysemous word exploration code here.

polysemous = wv_from_bin_pl.most_similar("blok")
for word, similarity in polysemous:
    print(word, similarity)

# Answers:
#
# Please state the polysemous word you discover and the multiple meanings
# that occur in the top 10.
# "blok" - "barak", "budynek", "kolumna", "sześcian", "bunkier", "sektor", "platforma",
#          "prostopadłościan", "segment", "panel"
#
# Why do you think many of the polysemous words you tried didn’t work?
# In my opinion this is because of the campus it was trained on, which isn't
# based on the modern language used by the average person.

# ------------------

#################################
# TODO: c)
# Synonyms & Antonyms
# ------------------
# Write your synonym & antonym exploration code here.

w1 = "szybki"
w2 = "śmigły"
w3 = "wolny"
w1_w2_dist = wv_from_bin_pl.distance(w1, w2)
w1_w3_dist = wv_from_bin_pl.distance(w1, w3)

print("Synonyms {}, {} have cosine distance: {}".format(w1, w2, w1_w2_dist))
print("Antonyms {}, {} have cosine distance: {}".format(w1, w3, w1_w3_dist))

# Answers
#
# Find three words (w1 , w2 , w3 ) where w1 and w2 are synonyms and w1 and w3 are antonyms,
# but Cosine Distance(w1 , w3) < Cosine Distance(w1 , w2).
# w1 = "szybki", w2 = "śmigły", w3 = "wolny"
# Cosine Distance(w1 , w3) = 0.6127157211303711
# Cosine Distance(w1 , w2) = 0.8197769224643707
#
# Please give a possible explanation for why this counterintuitive result may have happened.
# In my opinion it's because word "śmigły" is rarely used, so there aren't many occurs of this word,
# and thus it's representation might be not the best.


#################################
# TODO: d)
# Solving Analogies with Word Vectors
# ------------------

# ------------------
# Write your analogy exploration code here.
print()
pprint.pprint(wv_from_bin_pl.most_similar(
    positive=["król", "kobieta"], negative=["królowa"]))

# Answers
#
# Find an example of analogy that holds according to these vectors (i.e. the intended word
# is ranked top).
# królowa:kobieta :: król:mężczyzna
#
# Top 10:
# [('mężczyzna', 0.8137192726135254),
#  ('ludzie', 0.738577663898468),
#  ('człowiek', 0.7270129919052124),
#  ('osoba', 0.6618210673332214),
#  ('murzyn', 0.6415743231773376),
#  ('młodzieniec', 0.6330927014350891),
#  ('czyzn', 0.6061394810676575),
#  ('chłopiec', 0.605340838432312),
#  ('osobnik', 0.6032081842422485),
#  ('dziecko', 0.6003199219703674)]

#################################
# TODO: e)
# Incorrect Analogy
# ------------------
# Write your incorrect analogy exploration code here.
print()
pprint.pprint(wv_from_bin_pl.most_similar(
    positive=["tata", "kobieta"], negative=["mama"]))

# Answers
#
# Find an example of analogy that does not hold according to these vectors.
# mama:kobieta :: tata:mężczyzna
#
# Top 10:
# [('dziewczyna', 0.6737573146820068),
#  ('chłopiec', 0.6499072313308716),
#  ('dziewczynka', 0.6401896476745605),
#  ('chłopak', 0.6284945011138916),
#  ('dziecko', 0.6246432662010193),
#  ('szakila', 0.6002572178840637),
#  ('murzynka', 0.597832441329956),
#  ('dziewczę', 0.5963649749755859),
#  ('sonię', 0.5879225730895996),
#  ('staruszka', 0.5807310342788696)]

# ------------------


#################################
# TODO: f)
# Guided Analysis of Bias in Word Vectors
# Here `positive` indicates the list of words to be similar to and
# `negative` indicates the list of words to be most dissimilar from.
# ------------------
print()
pprint.pprint(wv_from_bin_pl.most_similar(
    positive=['kobieta', 'szef'], negative=['mezczyzna']))
print()
pprint.pprint(wv_from_bin_pl.most_similar(
    positive=['mezczyzna', 'prezes'], negative=['kobieta']))

# Answers
#
# Which terms are most similar to ”kobieta” and ”szef” and most dissimilar to ”mezczyzna”?
# The answer should be for example "szefowa".
# Top 10: 'własika', 'agent', 'oficer','esperów','interpol','antyterrorystyczny', 'komisarz',
#         'europolu', 'bnd', 'pracownik'
#
# Which terms are most similar to ”mezczyzna” and ”prezes” and most dissimilar to ”kobieta”?
# The answer should be for example "prezes".
# Top 10: 'wiceprezes', 'czlonkiem', 'przewodniczący', 'czlonek', 'przewodniczacym', 'wiceprzewodniczący',
#         'obowiazków', 'obowiazani', 'dyrektor', 'obowiazany'


#################################
# TODO: g)
# Independent Analysis of Bias in Word Vectors
# ------------------
print()
pprint.pprint(wv_from_bin_pl.most_similar(
    positive=['mezczyzna', 'wojownik'], negative=['kobieta']))

# Answers
#
# Briefly explain the example of bias that you discover.
# The word most similar to ”mezczyzna” and ”wojownik” and most dissimilar to ”kobieta” should be "wojownik,
# but we get in top 10:
# 'zolnierz', 'skrzydla', 'mezów', 'posrodku', 'ksztalt', 'szczatki', 'wladca',
#         'glowe', 'chlopiec', 'ganelon'

# TODO: h)
# The source of bias in word vectors
# ------------------

# Answers
#
# What might be the cause of these biases in the word vectors?
# In my opinion it's because some roles are associated with a gender, and
# thus one gender occurs more often in a given context.

#################################
# Section 3:
# English part
#################################
def load_word2vec():
    """ Load Word2Vec Vectors
        Return:
            wv_from_bin: All 3 million embeddings, each lengh 300
    """
    import gensim.downloader as api
    wv_from_bin = api.load("word2vec-google-news-300")
    vocab = list(wv_from_bin.key_to_index.keys())
    print("Loaded vocab size %i" % len(vocab))
    return wv_from_bin


wv_from_bin = load_word2vec()

#################################
# TODO:
# Find English equivalent examples for points b) to g).
#################################

# TODO: b)
# Polysemous Words
# ------------------
# Write your polysemous word exploration code here.

polysemous = wv_from_bin.most_similar("palm")
for word, similarity in polysemous:
    print(word, similarity)

# Answers:
#
# Please state the polysemous word you discover and the multiple meanings
# that occur in the top 10.
# "palm" - "palms", "lined_Croisette_waterfront", "Powered_Hard_Disk", "lauric_oils", "fatty_acid_distillate",
#          "palm_kernel_oils", "fingertip", "cupped_gently", "lined_boulevard", "coconut_oils"

# ------------------

#################################
# TODO: c)
# Synonyms & Antonyms
# ------------------
# Write your synonym & antonym exploration code here.

w1 = "old"
w2 = "elderly"
w3 = "young"
w1_w2_dist = wv_from_bin.distance(w1, w2)
w1_w3_dist = wv_from_bin.distance(w1, w3)

print("Synonyms {}, {} have cosine distance: {}".format(w1, w2, w1_w2_dist))
print("Antonyms {}, {} have cosine distance: {}".format(w1, w3, w1_w3_dist))

# Answers
#
# Find three words (w1 , w2 , w3 ) where w1 and w2 are synonyms and w1 and w3 are antonyms,
# but Cosine Distance(w1 , w3) < Cosine Distance(w1 , w2).
# w1 = "old", w2 = "elderly", w3 = "young"
# Cosine Distance(w1 , w3) = 0.5825658142566681
# Cosine Distance(w1 , w2) = 0.7541274130344391


#################################
# TODO: d)
# Solving Analogies with Word Vectors
# ------------------

# ------------------
# Write your analogy exploration code here.
print()
pprint.pprint(wv_from_bin.most_similar(
    positive=["king", "woman"], negative=["man"]))

# Answers
#
# Find an example of analogy that holds according to these vectors (i.e. the intended word
# is ranked top).
# man:king :: woman:queen
#
# Top 10:
# [('queen', 0.7118193507194519),
#  ('monarch', 0.6189674139022827),
#  ('princess', 0.5902431011199951),
#  ('crown_prince', 0.5499460697174072),
#  ('prince', 0.5377321839332581),
#  ('kings', 0.5236844420433044),
#  ('Queen_Consort', 0.5235945582389832),
#  ('queens', 0.5181134343147278),
#  ('sultan', 0.5098593831062317),
#  ('monarchy', 0.5087411999702454)]

#################################
# TODO: e)
# Incorrect Analogy
# ------------------
# Write your incorrect analogy exploration code here.
print()
pprint.pprint(wv_from_bin.most_similar(
    positive=["cat", "bark"], negative=["dog"]))

# Answers
#
# Find an example of analogy that does not hold according to these vectors.
# dog:bark :: cat:mew
#
# Top 10:
# [('frass', 0.5099735260009766),
#  ('cambium', 0.49994856119155884),
#  ('beetles_burrow', 0.4957101047039032),
#  ('chittering', 0.4945257604122162),
#  ('sapwood', 0.49447116255760193),
#  ('barky', 0.4896303415298462),
#  ('treefrogs', 0.4850967526435852),
#  ('sapsuckers', 0.4804511070251465),
#  ('tree_bark', 0.47680336236953735),
#  ('moth_caterpillars', 0.4735950529575348)]
# ------------------


#################################
# TODO: f, g)
# Guided Analysis of Bias in Word Vectors
# Here `positive` indicates the list of words to be similar to and
# `negative` indicates the list of words to be most dissimilar from.
# ------------------
print()
pprint.pprint(wv_from_bin.most_similar(
    positive=['man', 'manageress'], negative=['woman']))

# Answers
#
# Which terms are most similar to ”man” and ”manageress” and most dissimilar to ”woman”?
# The answer should be for example "manager".
# Top 10: 'shelf_stacker', 'Splott_Cardiff', 'Linthorpe_Road', 'kitchen_porter', 'Greggs_bakery',
#         'barman', 'Masked_raiders', 'Costcutters', 'bnd', 'publican'


# Load vectors for English and run similar analysis for points from b) to g). Have you observed
# any qualitative differences?

# The main differences is that results for analogies in english are better. It was harder to find wrong examples.
# I supposed, it's because used sources for text was better.
# The difference is also that, for homonyms, most synonyms were often for one meaning of the word.
