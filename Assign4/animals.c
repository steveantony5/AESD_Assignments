#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sort.h>
#include <uapi/linux/string.h>
#include <linux/list.h>
#include <linux/slab.h>

#define MAX (50)
//original array
static char animals[MAX][10] = {"monkey", "oxen", "monkey", "dino", "otter","monkey","otter","monkey","otter","cheetah",\
								 "porcupine", "anaconda", "white tiger","white tiger", "antelope", "cheetah", "bengal tiger", "otter", "panda", "bear",\
								 "porcupine", "panda", "otter", "bull", "goat", "cow", "bison", "cow", "bison", "monkey",\
								 };
static int count_animals[MAX];

//sorted arrays
static char animals_final[MAX][10];
static int count_animals_final[MAX];

//filtered temporary list
char animals_filtered[MAX][10];
int count_animals_filtered[MAX];


//linked list
struct animals{
	char name[10];
	struct list_head list;
	int count;
	};
/**********************************************************
 * Function name: Compare to sort in descending order
 * Parameters : the pointers to the two numbers which has to be compared
 * return : 1 if LHS < RHS
 * 	   -1 if LHS > RHS
 * *********************************************************/
int compare(const void *lhs, const void *rhs) 
{
	int cmp = 0;
    char *lhs_string = (const char *)(lhs);
    char *rhs_string = (const char *)(rhs);

 	if((cmp = strcmp(lhs_string,rhs_string)) >=0)
 	{
 		return 1;
 	}
 	else
 	{
 		return -1;
 	}
}


static int sort_init(void)
{
	int i = 0,final_count = 0, j =0, duplicate =0,l=0,p=0;
		int filter_index = 0;

	struct animals *temp;
  	struct animals filtered;
  	struct animals animalList;

  	INIT_LIST_HEAD(&animalList.list);

	int no_of_animals = (sizeof(animals) / sizeof(animals[0]));



	//Calls the linux built in sort function which does sort in O(nlogn)
	sort(animals,no_of_animals, 10,compare,NULL);

	for(i =0;i<no_of_animals;i++)
	{
		for(l =0;l<no_of_animals;l++)
		{
			if(strcmp(animals[i],animals[l]) == 0)
			{
				count_animals[i]++;
			}
		}
	}


	for( i =0; i<no_of_animals;i++)
	{
		for(j =0;j<no_of_animals;j++)
		{
			if(strcmp(animals[i],animals_final[j]) == 0)
			{
				duplicate = 1;
			}
		}



		if(duplicate != 1)
		{
			strcpy(animals_final[final_count],animals[i]);
			count_animals_final[final_count] = count_animals[i];

			//counting the number of occurances
			
			final_count++;

		}

		duplicate = 0;
		
	}


//Final animals list after removing duplicates
	i = 0;
	for(i=0;i<final_count;i++)
	{
		printk(KERN_ALERT "%s %d\n",animals_final[i],count_animals_final[i]);
	}


//converting animals_final array and count array to a linked list
	INIT_LIST_HEAD(&animalList.list);

	/* Inserting into a linked list */
  
   for(i=0; i<final_count; ++i)
   {
		temp= (struct animals *)kmalloc(sizeof(struct animals), GFP_KERNEL);
		
		if(temp !=NULL)
		{
			strcpy(temp->name,animals_final[i] );
			temp->count = count_animals_final[i];
  
			list_add_tail(&(temp->list), &(animalList.list));
		}
		else
		{
			printk("Error on allocating malloc\n");
		}


	}

	INIT_LIST_HEAD(&filtered.list);

	//filtering when count >2
	list_for_each_entry(temp, &animalList.list, list)
	{

		if((temp->count) > 2)
		{
			strcpy(animals_filtered[filter_index],(temp->name));
			count_animals_filtered[filter_index] = (temp->count);
			filter_index++;
		}
		
		
	}

	for(p=0;p<filter_index;p++)
	{
		temp= (struct animals *)kmalloc(sizeof(struct animals), GFP_KERNEL);
		if(temp != NULL)
		{
			strcpy(temp->name,animals_filtered[p] );
			temp->count = count_animals_filtered[p];
			list_add_tail(&(temp->list), &(filtered.list));
		}
		else
		{
			printk("Error on allocating malloc\n");
		}
	}

	printk("Final filtered list\n-------------\n");
	list_for_each_entry(temp, &filtered.list, list)
	{
		printk("Name = %s Count= %d\n", temp->name, temp->count);    
	}


	return 0;
}

static void sort_exit(void)
{
	printk(KERN_ALERT "Exiting!\n");
}


   

module_init(sort_init);
module_exit(sort_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steve Antony");
MODULE_DESCRIPTION("Module for timer");
MODULE_VERSION("0.01");


