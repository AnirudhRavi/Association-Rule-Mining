#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

//Structure of node of fp tree
typedef struct fp_node
{
  int id;
  int frequency;
  int children;
  struct fp_node **vertical_links;
  struct fp_node *horizontal_link;
  struct fp_node *parent_link;
}node_t;

typedef struct
{
  char *item_name;
  int item_count;
}item_t;

typedef struct
{
  int *item_ids;
  int num_items;
}trans_t;


//Function to create trees
void applyMinSupport(item_t *itemset, trans_t *numeric_transactions, int minsup, int num_items, int num_trans);
node_t **constructFPTree(item_t *itemset, trans_t *numeric_transactions, int num_trans, int num_items);
int checkChildren(node_t *tmphead, int id);

//Mining frequent item sets from FP Tree
void mineFrequentItemSetEndWith(node_t *root, int item, int *prefix, int num_items, int level);
node_t *createTreeEndWith(node_t *root, int item);
void setNullPrefix(int *prefix, int level, int num_items);
void *wmalloc(int size);
void freeTree(node_t *node);

//Functions for pre-processing data in data.csv
char* readContents(FILE *basket_file);
char **tokenize(char *astr, char a_delim, int *num);
int tokenizeItems(char **transactions,item_t **itemset, trans_t **numeric_transactions, int count);
void sortItems(item_t *itemset, trans_t transaction);
void exchange(int *a, int *b);
void removeSpaces(char *item_name);
void preProcess(char *filename);

FILE **fp;
int no_files=10;
void record(int *prefix, int frequency, int level);

int minsup;

//Crucial data strcutures
item_t *itemset;
trans_t *numeric_transactions;

int num_items, num_trans;
int no_lines;
int main(int argc, char **argv)
{
  int ii, jj;
  int tempint;
  float min_sup_p = 0.02;

  //Fill in data structures itemset and numeric_transactions
  preProcess("data.csv");

  node_t *head;
  node_t **head_pointers;
  num_trans = no_lines;
  minsup = (int)(min_sup_p*num_trans); //Absolute minimum count above which considered as frequent itemsets
  printf("\nMinimum Support Count: %d\n\n", minsup);

  head_pointers = constructFPTree(itemset, numeric_transactions, num_trans, num_items); //num_items is itemset_size (set in preProcess) 
  int *prefix = wmalloc(num_items*sizeof(int));
//Intialization of prefix array of size num_items to -1s
  setNullPrefix(prefix, -1, num_items);

  char *filename = malloc(100*sizeof(char));
  fp = wmalloc(no_files*sizeof(FILE *)); //no_files = 10, fp array of file pointers

  fp = wmalloc(4*sizeof(FILE*));
  for(ii=0;ii<4; ii++)
  {
    sprintf(filename, "%d_Frequent_Itemset", ii+1); //Generates 4 files with name *_Frequent_Itemset
    fp[ii] = fopen(filename, "w");
  }
  for(ii=0; ii<num_items; ii++)
  {
    mineFrequentItemSetEndWith(head_pointers[0], ii, prefix, num_items, 0); //Creating prefix tree for specific ID ii
    setNullPrefix(prefix, -1, num_items);
  }
  for(ii=0; ii<4; ii++)
    fclose(fp[ii]);
}

void preProcess(char *filename)
{
  FILE *basket_file = fopen(filename, "r");
  int ii, jj;
  char **transactions;
  char *file_contents = readContents(basket_file);
  transactions = tokenize(file_contents, '\n', &no_lines);
  no_lines--;
  num_items = tokenizeItems(transactions, &itemset, &numeric_transactions, no_lines); //populated global variables itemset and numeric_transcations; and setting num_items as itemset_size
  
  for(ii=0; ii<no_lines; ii++)  //sorting itemsets for each numeric transaction
     sortItems(itemset, numeric_transactions[ii]);
  

  for(ii=0; ii<num_items; ii++)
     removeSpaces(itemset[ii].item_name); //itemset names spaces replaced with underscore
}

void mineFrequentItemSetEndWith(node_t *root, int item, int *prefix, int num_items, int level)
{
  int i;
  node_t *new_tree = createTreeEndWith(root, item); //Prefix tree ending with item

  if(new_tree->frequency > minsup)
  {
    prefix[level] = item;
    record(prefix, new_tree->frequency, level+1);
    for(i=item-1; i>=0; i--)
    {
      mineFrequentItemSetEndWith(new_tree, i, prefix, num_items, level+1);
      setNullPrefix(prefix, level, num_items);
    }
  }

  freeTree(new_tree);
}

//DFS to create prefic tree ending with 'item'
node_t *createTreeEndWith(node_t *root, int item)
{
  node_t *head = wmalloc(sizeof(node_t));
  head->vertical_links = wmalloc(root->children*sizeof(node_t*)); //Prefix tree - Array with number of children
  int i, children=0, count;
  head->frequency=0;
  head->id = root->id;
  for(i=0; i<root->children; i++)
  {
    count = countEndingWith(root->vertical_links[i], item);
    if(count>0)
    {
      head->vertical_links[children++] = createTreeEndWith(root->vertical_links[i], item);
    }
    head->frequency += count;
  }
  head->children =  children;
  head->vertical_links = realloc(head->vertical_links, children*sizeof(node_t*));
  return head;
}

//Find number of branches ending with item 'item'
int countEndingWith(node_t *root, int item)
{
  if(root->id == item)
    return root->frequency;
  int i, count=0;
  for(i=0; i < root->children; i++)
  {
    count += countEndingWith(root->vertical_links[i], item);
  }
  return count;
}


void setNullPrefix(int *prefix, int level, int num_items)
{
  int ii;
  for(ii=num_items; ii>level; ii--)
  {
    prefix[ii] = -1;
  }
}

void *wmalloc(int size)
{
  void *pointer;
  if((pointer= malloc(size))==NULL)
  {
    printf("Could not allocate memory\n");
    exit(0);
  }
  return pointer;
}

void applyMinSupport(item_t *itemset, trans_t *numeric_transactions, int minsup, int num_items, int num_trans)
{
  int ii, jj, kk;
  for(kk=0; kk<num_items; kk++)
  {
    if(itemset[kk].item_count<minsup)
    {
      for(ii=0; ii<num_trans; ii++)
      {
	for(jj=0; jj<numeric_transactions[ii].num_items;jj++)
	{
	  if(numeric_transactions[ii].item_ids[jj]==kk)
	  {
	    numeric_transactions[ii].item_ids = realloc(numeric_transactions[ii].item_ids, jj*sizeof(int));
	    numeric_transactions[ii].num_items = jj;
	    break;
	  }
	}
      }
    }
  }
}

node_t **constructFPTree(item_t *itemset, trans_t *numeric_transactions, int num_trans, int num_items)
{
  int ii, jj, k;
  node_t **head_pointers = wmalloc((num_items+1)*sizeof(node_t*));
  node_t *head = wmalloc(sizeof(node_t));
  //Intialiazing node with basic values for head[0]
  head->id = -1;		
  head->frequency = 0;
  head->children=0;
  head->vertical_links = wmalloc(sizeof(node_t*));
  head->vertical_links[0] = NULL;
  head->parent_link = NULL;
  head->horizontal_link = NULL;
  node_t *iter, *tmp, *horizont_traversal;
  //Intializing the headpointer array by iterating through all unique items in itemsets
  for(ii=1; ii<=num_items; ii++) 
  {
    head_pointers[ii]=wmalloc(sizeof(node_t));
    head_pointers[ii]->id = ii-1;
    head_pointers[ii]->frequency=0;
    head_pointers[ii]->children=0;
    head_pointers[ii]->vertical_links = wmalloc(sizeof(node_t*));
    head_pointers[ii]->vertical_links[0] = NULL;
    head_pointers[ii]->horizontal_link = NULL;
    head_pointers[ii]->parent_link = NULL;
  }

  for(ii=0; ii<num_trans-1; ii++)
  {
    iter = head;
    for(jj=0; jj<numeric_transactions[ii].num_items; jj++)
    {
      k=checkChildren(iter, numeric_transactions[ii].item_ids[jj]);
      //Creating a new child node
      if(k<0)
      {
	tmp = wmalloc(sizeof(node_t));
	tmp->id = numeric_transactions[ii].item_ids[jj];
	tmp->frequency=1;
	tmp->children=0;
	tmp->vertical_links=wmalloc(sizeof(node_t*));
	tmp->vertical_links[0]=NULL;
	tmp->horizontal_link=NULL;
	tmp->parent_link = iter;
	if(head_pointers[tmp->id + 1]->horizontal_link == NULL) //If only unique item in tree sets horizontal link for item
	{
	  head_pointers[tmp->id+1]->horizontal_link = tmp;
	  head_pointers[tmp->id+1]->frequency++;
	}
	else //Sets horizontal pointer across tree
	{
	  horizont_traversal = head_pointers[tmp->id + 1];
	  head_pointers[tmp->id + 1]->frequency++;
	  while(horizont_traversal->horizontal_link != NULL)
	    horizont_traversal = horizont_traversal->horizontal_link;
	  horizont_traversal->horizontal_link = tmp;
	}
        //Create vertical links
	iter->vertical_links = realloc(iter->vertical_links, (iter->children+1)*sizeof(node_t*)); //Increasing child array by 1 element
	iter->vertical_links[iter->children] = tmp;
	iter->children++;
	iter=tmp;
      }
      //Updating frequency of existing child
      else
      {
	iter = iter->vertical_links[k];
	iter->frequency++; //Updating fptree element with new frequency
	head_pointers[iter->id + 1]->frequency++; //Updating in the hash table data structure with new frequency
      }

    }
  }
  head_pointers[0] = head;
  return head_pointers;
}

//Returns -1 if no immediate children's id != id; or no immediate children; else returns index of child with same id
int checkChildren(node_t *tmphead, int id)
{
  int num_child = tmphead->children;
  int i;
  for(i=0; i<num_child; i++)
    if(tmphead->vertical_links[i]->id==id)
      return i;

  return -1;

}

void freeTree(node_t *node)
{
  int i;
  if(node->children==0)
    return;
  for(i=0; i<node->children; i++)
  {
    freeTree(node->vertical_links[i]);
    free(node->vertical_links[i]);
  }
  free(node->vertical_links);
}

//Based on level - putting frequent itemsets in the respective file
void record(int *prefix, int frequency, int level)
{
  int i=0;

  fprintf(fp[level>4?3:level-1], "%d ", level);
  while(prefix[i] != -1)
  {
    fprintf(fp[level>4?3:level-1], "%d ", prefix[i]);
    i++;
  }
  fprintf(fp[level>4?3:level-1], "%d\n", frequency);
}


///Bubble sorting in order to get all items in transcations in sorted order of number of times it has occurred
void sortItems(item_t *itemset, trans_t transaction)
{
  int n=transaction.num_items;
  if(n==1)
    return;
  int i, done;
  do
  {
    done=1;
    for(i=0; i<n-1; i++)
    {
      if(itemset[transaction.item_ids[i]].item_count<itemset[transaction.item_ids[i+1]].item_count)
      {
	//printf("Items to be exchanged : %s, %s\n", itemset[transaction.item_ids[i]].item_name, itemset[transaction.item_ids[i+1]].item_name);
	exchange(transaction.item_ids + i, transaction.item_ids + i+ 1);
	done=0;
      }
    }

  }while(done==0);
}
void exchange(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

int tokenizeItems(char **transactions,item_t **add_itemset, trans_t **add_numeric_transactions, int count) //count = no of transcations
{
  int ii,jj, no_items;
  *add_numeric_transactions = malloc(count*sizeof(trans_t));
  trans_t *numeric_transactions = *add_numeric_transactions;
  char **tmp = tokenize(transactions[0], ',', &no_items); //Number of items in each transaction
  no_items--;
  numeric_transactions[0].item_ids = malloc(no_items*sizeof(int)); //Create array to store item ids
  numeric_transactions[0].num_items=no_items; //storing number of items
  int item_set_size=0, tmp_pos;
  *add_itemset = malloc(no_items*sizeof(item_t)); //creating itemset for transcation
  item_t *itemset = *add_itemset;
  //Creating itemset and numeric_transcations for the 1st transaction
  for(jj=0; jj<no_items; jj++)
  {
    itemset[jj].item_name = strdup(tmp[jj]);
    itemset[jj].item_count=1;
    numeric_transactions[0].item_ids[jj]=item_set_size; //Unique IDs for each item in the 1st transactions
    item_set_size++;
  }
  //Generating itemset and numeric_transcations for remaining transcations
  for(ii=1; ii<count; ii++)
  {
    tmp = tokenize(transactions[ii], ',', &no_items);
    no_items--;
    numeric_transactions[ii].item_ids = malloc(no_items*sizeof(int));
    numeric_transactions[ii].num_items=no_items;
    for(jj=0; jj<no_items; jj++)
    {
      tmp_pos = checkItem(itemset, tmp[jj], item_set_size);  //Preventing duplicates in itemset

      //If unique item then add to itemset
      if(tmp_pos<0)
      {
	*add_itemset = realloc(*add_itemset, (item_set_size+1)*sizeof(item_t));
	itemset = *add_itemset;
	itemset[item_set_size].item_name = strdup(tmp[jj]);
	itemset[item_set_size].item_count = 1;
	numeric_transactions[ii].item_ids[jj]=item_set_size;
	item_set_size++;
      }
      //Else increase count of the existing item
      else
      {
	itemset[tmp_pos].item_count++;
	numeric_transactions[ii].item_ids[jj]=tmp_pos;
      }
    }
  }
  return item_set_size;
}

int checkItem(item_t *itemset, char *item, int item_set_size)
{
  int i;
  for(i=0; i<item_set_size; i++)
  {
    if(strcmp(itemset[i].item_name, item)==0)
      return i;
  }
  return -1;

}

//Creating a heap data structure from the file stream structure on heap
char **tokenize(char *a_str, char a_delim, int *num)  
{
  char *tmp=a_str;
  char **result=0;
  char delim[2];
  char* last_comma = 0;
  size_t count=0;
  delim[0] = a_delim;
  delim[1] = 0;
  while (*tmp)
  {
      if (a_delim == *tmp)
      {
          count++;
          last_comma = tmp;
      }
      tmp++;
  }
  count += last_comma < (a_str + strlen(a_str) - 1);
  count++;
  result = malloc((count)*(sizeof(char*)));
  if (result)
  {
    size_t idx  = 0;
    char* token = strtok(a_str, delim);

    while (token)
    {
        assert(idx < count);
        *(result + idx++) = strdup(token);
        token = strtok(0, delim);
    }
    assert(idx == count -1);
    *(result + idx) = 0;
  }
  *num = count;
  return result;

}

//Dumps the file stream to a heap and returns the pointer
char* readContents(FILE *basket_file)  
{
  char *file_contents;
  FILE *input_file = basket_file;
  long input_file_size;
  fseek(input_file, 0, SEEK_END);
  input_file_size = ftell(input_file);
  rewind(input_file);
  file_contents = malloc(input_file_size * (sizeof(char)));
  fread(file_contents, sizeof(char), input_file_size, input_file);
  return file_contents;
}

//Replacing spaces between item names with underscore
void removeSpaces(char *item_name)
{
  char *tmp = item_name;
  while(*tmp!='\0')
  {
    if(*tmp==' ')
      *tmp='_';
    tmp++;
  }
}
