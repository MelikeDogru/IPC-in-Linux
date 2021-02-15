#define ProjectId 321
#define Path  "messagequeue.h" 
#define MessageLen    4
#define MessageCount  6

typedef struct { 
  long type;                 
  char payload[MessageLen + 1];   
} queuedMessage;