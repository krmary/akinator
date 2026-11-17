#define assert_my(condition, red_param) \
    { \
        if(! condition) \
        { \
            fprintf(stderr, "HELLO\nerror:( : condition: " #condition ",\n\
            %s:%d function: %s, ", __FILE__, __LINE__, __func__); \
            return red_param; \
        } \
    }




