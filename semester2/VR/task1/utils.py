import torch


DEBUG=False

i = 0

def debug(tensor, name):
    if DEBUG:
        global i
        print(f"{i} {name}", tensor.shape, type(tensor))
        print("min, mean, max:")
        print(tensor.min(), tensor.float().mean(), tensor.max())
        i += 1


def flatten_boxes_tensor(boxes):
    """
    :param boxes: format [num_boxes, coords(4), H, W]
    :return: format [num_boxes, coords(4)]
    """
    flat = torch.reshape(boxes, [4, -1]).permute([1, 0])
    return flat
