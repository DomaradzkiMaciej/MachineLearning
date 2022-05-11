# Calculate the accuracy of a baseline that simply predicts "London" for every
#   example in the dev set.
# Hint: Make use of existing code.
# Your solution here should only be a few lines.

import argparse
from utils import evaluate_places


argp = argparse.ArgumentParser()
argp.add_argument('--eval_corpus_path',
    help="Path of the corpus to evaluate on", default=None)
args = argp.parse_args()

with open(args.eval_corpus_path, 'r') as f:
    corpus_len = len(f.readlines())

predictions = ['London'] * corpus_len
total, correct = evaluate_places(args.eval_corpus_path, predictions)
print('Correct: {} out of {}: {}%'.format(correct, total, correct / total * 100))
