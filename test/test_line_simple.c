/* Test for #line directive - simpler version */

/* Test basic line directive */
#line 100
/* This comment should be considered at line 100 */

/* Test line directive with filename */
#line 200 "virtual.c"
/* This comment should be considered at line 200 in virtual.c */

/* Test line directive with only number */
#line 50
/* This comment should be considered at line 50 */

/* Test with spaces */
#line    300    "test.c"   
/* This comment should be at line 300 in test.c */

/* Reset to line 1 */
#line 1 "final.c"
/* Back to line 1 in final.c */
