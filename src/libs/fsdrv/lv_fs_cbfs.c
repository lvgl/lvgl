// this file should not exist
#ifdef __GNUC__
    #define IS_NOT_USED __attribute__ ((unused))
#else
    #define IS_NOT_USED
#endif
IS_NOT_USED static void nothing(void)
{
    // do nothing
}
