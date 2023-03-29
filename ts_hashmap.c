#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ts_hashmap.h"
static pthread_mutex_t * map_index_mutex;
/**
 * Creates a new thread-safe hashmap. 
 *
 * @param capacity initial capacity of the hashmap.
 * @return a pointer to a new thread-safe hashmap.
 */
ts_hashmap_t *initmap(int capacity) {
  // TODO
  ts_hashmap_t* map = (ts_hashmap_t*)(malloc(sizeof(ts_hashmap_t)));
  map->capacity = capacity;
  map->table = (ts_entry_t**)(malloc(sizeof(ts_entry_t*)*capacity));
  map_index_mutex = (pthread_mutex_t*)(malloc(sizeof(pthread_mutex_t)*capacity));
  for(int i =0;i<capacity;i++){
    map->table[i] = NULL;
    int ret = pthread_mutex_init(&map_index_mutex[i],NULL);
  }
  map->size = 0;

  return map;
}

/**
 * Obtains the value associated with the given key.
 * @param map a pointer to the map
 * @param key a key to search
 * @return the value associated with the given key, or INT_MAX if key not found
 */
int get(ts_hashmap_t *map, int key) {
  // TODO
  uint expectedIndex = (uint)(((uint)key)%map->capacity);
  pthread_mutex_lock(&map_index_mutex[expectedIndex]);
  ts_entry_t* entry = map->table[expectedIndex];
  while(entry !=NULL){
    int entryKey = entry->key;
    if(entryKey==key){
        pthread_mutex_unlock(&map_index_mutex[expectedIndex]);
      return entry->value;
    }
    entry = entry->next;
  }
  pthread_mutex_unlock(&map_index_mutex[expectedIndex]);
  return INT_MAX;
}

/**
 * Associates a value associated with a given key.
 * @param map a pointer to the map
 * @param key a key
 * @param value a value
 * @return old associated value, or INT_MAX if the key was new
 */
int put(ts_hashmap_t *map, int key, int value) {
  // TODO
    uint expectedIndex = (uint)(((uint)key)%map->capacity);
    pthread_mutex_lock(&map_index_mutex[expectedIndex]);
  ts_entry_t *entry = map->table[expectedIndex];
  ts_entry_t* prevEntry = NULL;
  while(entry !=NULL){
    int entryKey = entry->key;
    if(entryKey==key){
      int oldValue = entry->value;
      entry->key = key;
      entry->value = value;
      pthread_mutex_unlock(&map_index_mutex[expectedIndex]);
      return oldValue;
    }
    prevEntry = entry;
    entry = entry->next;
  }
  ts_entry_t* newEntryPtr = (ts_entry_t*)malloc(sizeof(ts_entry_t));
  newEntryPtr->key = key;
  newEntryPtr->value = value;
  newEntryPtr->next = NULL;
  if(prevEntry!=NULL){
    prevEntry->next = newEntryPtr;
  } else{
    map->table[expectedIndex] = newEntryPtr;
  }
  map->size++;
  pthread_mutex_unlock(&map_index_mutex[expectedIndex]);
  return INT_MAX;
}

/**
 * Removes an entry in the map
 * @param map a pointer to the map
 * @param key a key to search
 * @return the value associated with the given key, or INT_MAX if key not found
 */
int del(ts_hashmap_t *map, int key) {
  // TODO
  uint expectedIndex = (uint)(((uint)key)%map->capacity);
  pthread_mutex_lock(&map_index_mutex[expectedIndex]);
  ts_entry_t* entry = map->table[expectedIndex];
  ts_entry_t* previousEntry= NULL;
  while(entry !=NULL){
    int entryKey = entry->key;
    if(entryKey==key){
      int oldValue = entry->value;
      if(previousEntry!=NULL){
        previousEntry->next = entry->next;
      } else{
        map->table[expectedIndex] = entry->next;
      }
      free(entry);
      entry = NULL;
      map->size--;
      pthread_mutex_unlock(&map_index_mutex[expectedIndex]);
      return oldValue;
    }
    previousEntry = entry;
    entry = entry->next;
  }
  pthread_mutex_unlock(&map_index_mutex[expectedIndex]);
  return INT_MAX;
}


/**
 * @return the load factor of the given map
 */
double lf(ts_hashmap_t *map) {
  return (double) map->size / map->capacity;
}

/**
 * Prints the contents of the map
 */
void printmap(ts_hashmap_t *map) {
  for (int i = 0; i < map->capacity; i++) {
    printf("[%d] -> ", i);
    ts_entry_t *entry = map->table[i];
    while (entry != NULL) {
      printf("(%d,%d)", entry->key, entry->value);
      if (entry->next != NULL)
        printf(" -> ");
      entry = entry->next;
    }
    printf("\n");
  }
}

  void freemap(ts_hashmap_t *map){
    for(int i = 0; i<map->capacity; i++){

        ts_entry_t* entry = map->table[i];
        ts_entry_t* prevEntry = NULL;
        if(entry==NULL){
          free(entry);
          entry = NULL;
        } else{
        while(entry!=NULL){
          prevEntry= entry;
          entry = entry->next;
          free(prevEntry);
          prevEntry = NULL;
        }
        }
    }
    free(map->table);
    map->table = NULL;
    free(map);
    free(map_index_mutex);
  }
  void * parTest(void* voidArgs){
    threadArgs* args = (threadArgs*)voidArgs;
    int numThreads = args->numThreads;
    int thread = args->currthread;
    ts_hashmap_t* map = args->map;
    for(int i = 0;i<1000/numThreads;i++){
      int random = rand();
      int command = random%3;
		int keyValue = random%100;
		if(command ==0){
			put(map,keyValue,keyValue);
		} else if(command==1){
			del(map,keyValue);
		} else if(command ==2){
			get(map,keyValue);
		}
    }
    free(args);
    return NULL;
  }