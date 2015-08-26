#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

typedef struct
{
  int cardinality;
  int *ids;
  int frequency;
}freq_itemsets_t;
char* readContents(FILE *freq_item_file);
int countLines(FILE *basket_file);
void sortItems(freq_itemsets_t *itemsets, int no_items, int card);
void exchange(freq_itemsets_t *a, freq_itemsets_t *b);

int main()
{
  char *filenames[]={"1_Frequent_Itemset", "2_Frequent_Itemset", "3_Frequent_Itemset", "4_Frequent_Itemset"};
  int i, no_lines, j, k, tmp_int;
  char *file_contents;
  FILE *fp ,*freq_sets = fopen("All_Frequent_Itemsets.txt", "w");
  freq_itemsets_t *itemsets;
  
  for(i=1; i<=4; i++)
  {
    fp = fopen(filenames[i-1], "r");
    if(fp==NULL)
    {
      printf("Could not open file\n");
      exit(0);
    }
    printf("Processing file %s\n", filenames[i-1]);
    no_lines = countLines(fp);
    rewind(fp);
    printf("No of items is %d\n", no_lines);
    itemsets = malloc(no_lines*sizeof(freq_itemsets_t));
    for(j=0; j<no_lines; j++)
    {
      itemsets[j].ids = malloc(i*sizeof(int));
      fscanf(fp, "%d", &itemsets[j].cardinality);
      for(k=0; k<i; k++)
      {
	fscanf(fp, "%d", &itemsets[j].ids[k]);
      }
      fscanf(fp, "%d", &itemsets[j].frequency);
    }
    sortItems(itemsets, no_lines, i );
    for(j=0; j<no_lines; j++)
    {
      fprintf(freq_sets, "%d ", i);
      for(k=0; k<i; k++)
      {
	fprintf( freq_sets, "%d ",itemsets[j].ids[k] );
      }
      fprintf(freq_sets, "%d\n", itemsets[j].frequency);
    }
    free(itemsets);
    fclose(fp);
   }
   fclose(freq_sets);
}

void sortItems(freq_itemsets_t *itemsets, int no_items, int card)
{
  int i, done;
  do
  {
    done=1;
    for(i=0; i<no_items-1; i++)
    {
      if(itemsets[i].frequency<itemsets[i+1].frequency)
      {
	exchange(itemsets + i, itemsets + i +1);
	done=0;
      }
    }
  }while(done==0);
}

void exchange(freq_itemsets_t *a, freq_itemsets_t *b)
{
  freq_itemsets_t tmp;
  tmp.frequency = a->frequency;
  tmp.ids = a->ids;
  a->frequency = b->frequency;
  a->ids = b->ids;
  b->frequency = tmp.frequency;
  b->ids = tmp.ids;
}

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

int countLines(FILE *basket_file)
{
  FILE *tmp = basket_file;
  char chr = getc(tmp);
  int count_lines=0;
  while(chr!=EOF)
  {
    if(chr=='\n')
      count_lines++;
    chr = getc(tmp);
  }
  
  return count_lines;
}
