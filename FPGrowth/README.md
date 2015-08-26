**Program**: Programs that implement FP Growth algorithm logic to find frequent item sets and then get corresponding association rules 

**Author**: Anirudh Ravi

**About**:

These programs are written in C. The algorithm constructs a conditional frequent pattern tree and conditional pattern base from the data set which satisfies minimum support.

**Memory Utilization**:

Due to compact structure and no candidate generation requires less memory.

**Data Set**:

The Groceries data set contains 1 month (30 days) of real-world point-of-sale transaction data from a typical local grocery outlet. The data set contains 9835 transactions and the items are aggregated to 169 categories. The data set is provided for arules by Michael Hahsler, Kurt Hornik and Thomas Reutterer.

**Rules Generated**:
```
7 11 -> 5 Confidence: 30.978261
7 5 -> 11 Confidence: 41.379310
11 5 -> 7 Confidence: 53.395785
7 11 -> 42 Confidence: 30.978261
7 42 -> 11 Confidence: 47.401247
11 42 -> 7 Confidence: 48.927039
```

**Program 1: FPtree.c**

Preprocesses data present in data.csv, constructs an FP Tree and stores Frequent Item Sets based on level.

**Program 2: Postprocess.c**

Postprocessing files saved based on level of Frequent Item Sets into a single file. 

**Program 3: AssociationRule.c**

Creates association rules based on the frequent item sets.

**Executing**:

cc FPtree.c -o FPtree

./FPtree

[Creates 4 files named *_Frequent_Itemset *=1,2,3,4]

cc Postprocess.c -o Postprocess

./Postproces

[Combines all *_Frequent_Itemset into a single file All_Frequent_Itemsets.txt]

cc AssociationRule.c -o AssociationRule -lm

./AssociationRule

[Creates association rules based on the frequent itemsets]
