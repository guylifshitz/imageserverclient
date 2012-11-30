#define BUFF_SIZE (1024)   /* for client read/write */
#define BACKLOG     (12)   /* maximum number of concurrent clients */
enum {false, true};        /* 0 and 1, respectively */
typedef unsigned bool;     /* bool aliases unsigned int */
int ae_load_file_to_memory(const char *filename, char **result);
void generate_image_response(char request[ ], char response[ ]);
