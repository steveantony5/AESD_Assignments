/**************************************************************************
*						INCLUDES
**************************************************************************/
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sort.h>
#include <uapi/linux/string.h>
#include <linux/list.h>
#include <linux/slab.h>

/**************************************************************************
*						MACROS
**************************************************************************/
#define MAX (50) //max no of animals in the ecosystem

/**************************************************************************
*						Module Information
**************************************************************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steve Antony");
MODULE_DESCRIPTION("Module for animal ecosystem");
MODULE_VERSION("0.01");

/**************************************************************************
*			Local Variables default value when no parameter passed
**************************************************************************/
static int filter_count = 1;
static char *filter_animal = "default";



//original array
static char animals[MAX][50] = {"monkey", "oxen", "monkey", "dino", "otter","monkey","otter","monkey","otter","cheetah",\
								 "porcupine", "anaconda", "white tiger","white tiger", "antelope", "cheetah", "bengal tiger", "otter", "panda", "bear",\
								 "porcupine", "panda", "otter", "bull", "goat", "cow", "bison", "cow", "bison", "monkey",\
								 "whale", "whale", "penquin", "lamb", "pig", "rabbit", "rabbit", "panda", "panda", "panda",\
								 "oxen", "mule", "whale", "dino", "otter", "bison", "bear", "otter", "bison", "bison"};
static int count_animals[MAX];

//sorted arrays
static char animals_final[MAX][50];
static int count_animals_final[MAX];
static struct animals *temp;

static struct animals filtered;
static struct animals animalList;

//filtered temporary list
static char animals_filtered[MAX][50];
static int count_animals_filtered[MAX];

//linked list for the zoo ecosystem
struct animals{
	char name[50];
	struct list_head list;
	int count;
	};

/**************************************************************************
*						Module Parameters
**************************************************************************/

module_param(filter_animal, charp, 0644);
module_param(filter_count, int, S_IRUSR | S_IRGRP| S_IROTH| S_IWUSR);

/**********************************************************
 * Function name: Compare to sort in descending order
 * Parameters : the pointers to the two numbers which has to be compared
 * return : 1 if LHS < RHS
 * 	   -1 if LHS > RHS
 * *********************************************************/
int compare(const void *lhs, const void *rhs) 
{
	int cmp = 0;
    char *lhs_string = (char *)(lhs);
    char *rhs_string = (char *)(rhs);

 	if((cmp = strcmp(lhs_string,rhs_string)) >=0)
 	{
 		return 1;
 	}
 	else
 	{
 		return -1;
 	}
}

/**************************************************************************
*						Init Module 
**************************************************************************/
static int sort_init(void)
{


	int i = 0,final_count = 0, j =0, duplicate =0,l=0,p=0;
	int filter_index = 0;


	// finding total no of animals in the ecosystem
	int no_of_animals = (sizeof(animals) / sizeof(animals[0]));



	//Calls the linux built in sort function which does sort in O(nlogn)
	sort(animals,no_of_animals, 50,compare,NULL);


	//counting the no of each animals and stroing the count in count_animals array
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

	//removing the duplicate animals from the array
	for( i =0; i<no_of_animals;i++)
	{
		for(j =0;j<no_of_animals;j++)
		{
			if(strcmp(animals[i],animals_final[j]) == 0)
			{
				duplicate = 1;
			}
		}


		//copying to a temporary array which doesn't contain duplicate animals
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

	/* Inserting sorted non duplicate array into a linked list */
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
	printk("The number of elements of set 1 is %d\n",final_count );

	printk("The total amount of memory allocated for set 1 is %d\n",(final_count * sizeof(struct animals)));

	INIT_LIST_HEAD(&filtered.list);

	//generating the filtered array based on the filter conditions
	list_for_each_entry(temp, &animalList.list, list)
	{

		if(strcmp(filter_animal,"default") == 0)
		{

			if((temp->count) >= filter_count)
			{
				strcpy(animals_filtered[filter_index],(temp->name));
				count_animals_filtered[filter_index] = (temp->count);
				filter_index++;
			}
		}
		else
		{
			if(((temp->count) >= filter_count) && (strcmp(filter_animal,(temp->name)) == 0))
			{
				strcpy(animals_filtered[filter_index],(temp->name));
				count_animals_filtered[filter_index] = (temp->count);
				filter_index++;
			}
		}
		
		
	}

	//copying the filtered array to a filtered link list
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

	//Printing the filtered list
	printk("Filtered list\n-------------\n");
	list_for_each_entry(temp, &filtered.list, list)
	{
		printk("Name = %s Count= %d\n", temp->name, temp->count);    
	}
	printk("The number of elements of set 2 is %d\n",filter_index );

	printk("The total amount of memory allocated for set 2 is %d\n",(filter_index * sizeof(struct animals)));

	return 0;
}

/**************************************************************************
*						Exit Module 
**************************************************************************/

static void sort_exit(void)
{
	int count_sort=0;
	int count_filter=0;

	printk(KERN_ALERT "Exiting!\n");

	// free the nodes of linked list
	list_for_each_entry(temp, &animalList.list, list)
	{
		count_sort++;
		kfree(temp);
	}
	list_for_each_entry(temp, &filtered.list, list)
	{
		count_filter++;
		kfree(temp);
	}
	printk("Freed all nodes of linked list\n");
	printk("The total amount of memory freed is %d\n",((count_sort+count_filter) * sizeof(struct animals)));
}


   

module_init(sort_init);
module_exit(sort_exit);


