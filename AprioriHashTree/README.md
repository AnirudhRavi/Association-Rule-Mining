**Program**: Programs that implement Apriori algorithm logic through Hash tree structure to find frequent item sets and then get corresponding association rules 

**Author**: Anirudh Ravi

**About**:

These programs are written in C. The algorithm makes use of Hash Tree to calculate frequent item sets through Apriori approach and then generates Assocation rules for these itemsets.

**Data Set**:

The Groceries data set contains 1 month (30 days) of real-world point-of-sale transaction data from a typical local grocery outlet. The data set contains 9835 transactions and the items are aggregated to 169 categories. The data set is provided for arules by Michael Hahsler, Kurt Hornik and Thomas Reutterer.

**Rules Generated**:

7 11 -> 5 Confidence: 30.978261
7 5 -> 11 Confidence: 41.379310
11 5 -> 7 Confidence: 53.395785
7 11 -> 42 Confidence: 30.978261
7 42 -> 11 Confidence: 47.401247
11 42 -> 7 Confidence: 48.927039

**Program 1: Hashtree.c**

Reads and stores data in market_output.txt for transcations, and calculates frequent itemsets through Apriori approach usinga Hash tree

**Program 2: AssociationRule.c**

Creates association rules based on the frequent item sets.

**Executing**:

cc Hashtree.c -o Hashtree
./Hashtree
[Creates a output file - frequentItemSetData.txt]

cc AssociationRule.c -o AssociationRule -lm
./AssociationRule
[Creates association rules based on the frequent itemsets]
