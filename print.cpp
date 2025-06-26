/* @(#)print.c	2.1.8.2 */
/* generate flat files for data load */

#include <torch/torch.h>
#include "dss.h"

#include <stdio.h>
#ifndef VMS
#include <sys/types.h>
#endif

#if defined(SUN)
#include <unistd.h>
#endif

#if defined(LINUX)
#include <unistd.h>
#endif /*LINUX*/

#include <math.h>

#include "dss.h"
#include "dsstypes.h"
#include <string.h>

#include <stdio.h>
#include <time.h>

#include <stdint.h>

using STR_t = int8_t;
using DATE_t = int32_t;
using INT_t = int64_t;
using FLOAT_t = double;

constexpr auto TQP_DATE_TYPE = torch::kInt32;
constexpr auto TQP_INT_TYPE = torch::kInt64;
constexpr auto TQP_FLOAT_TYPE = torch::kFloat64;
constexpr auto TQP_STR_TYPE = torch::kInt8;

DATE_t convert(const char *date)
{
    int year, month, day;
    sscanf(date, "%d-%d-%d", &year, &month, &day);
    
    // Calculate the total number of days since the epoch 1990-01-01.
    static int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    DATE_t days = (year - 1990) * 365 + (year > 1992) + (year > 1996);
    for (int m = 0; m < month - 1; ++ m) {
        days += days_in_month[m];
    }
    if (month > 2 && (year == 1992 || year == 1996))
        days += 1;
    days += day - 1;
    return days;
}

/*
 * Function Prototypes
 */
FILE *print_prep PROTO((int table, int update));
int pr_drange PROTO((int tbl, long min, long cnt, long num));

FILE *
print_prep(int table, int update)
{
	char upath[128];
	FILE *res;

	if (updates)
		{
		if (update > 0) /* updates */
			if ( insert_segments )
				{
				int this_segment;
				if(strcmp(tdefs[table].name,"orders.tbl"))
					this_segment=++insert_orders_segment;
				else 
					this_segment=++insert_lineitem_segment;
				sprintf(upath, "%s%c%s.u%d.%d", 
					env_config(PATH_TAG, PATH_DFLT),
					PATH_SEP, tdefs[table].name, update%10000,this_segment);
				}
			else
				{
				sprintf(upath, "%s%c%s.u%d",
				env_config(PATH_TAG, PATH_DFLT),
				PATH_SEP, tdefs[table].name, update);
				}
		else /* deletes */
			if ( delete_segments )
				{
				++delete_segment;
				sprintf(upath, "%s%cdelete.u%d.%d",
					env_config(PATH_TAG, PATH_DFLT), PATH_SEP, -update%10000,
					delete_segment);
				}
			else
				{
				sprintf(upath, "%s%cdelete.%d",
				env_config(PATH_TAG, PATH_DFLT), PATH_SEP, -update);
				}
		return(fopen(upath, "w"));
        }
    res = tbl_open(table, "w");
    // OPEN_CHECK(res, tdefs[table].name);
    return(res);
}

int
dbg_print(int format, FILE *target, void *data, int len, int sep)
{
// 	int dollars,
// 		cents;

// 	switch(format)
// 	{
// 	case DT_STR:
// 		if (columnar)
// 			fprintf(target, "%-*s", len, (char *)data);
// 		else
// 			fprintf(target, "\"%s\"", (char *)data);
// 		break;
// #ifdef MVS
// 	case DT_VSTR:
// 		/* note: only used in MVS, assumes columnar output */
// 		fprintf(target, "%c%c%-*s", 
// 			(len >> 8) & 0xFF, len & 0xFF, len, (char *)data);
// 		break;
// #endif /* MVS */
// 	case DT_INT:
// 		if (columnar)
// 			fprintf(target, "%12ld", (long)data);
// 		else
// 			fprintf(target, "%ld", (long)data);
// 		break;
// 	case DT_HUGE:
// #ifndef SUPPORT_64BITS
//         if (*(long *)((long *)data + 1) == 0) \
//            if (columnar) fprintf(target, "%12ld", *(long *)data);
//            else fprintf(target, "%ld", *(long *)data);
//         else
//            if (columnar) fprintf(target, "%5ld%07ld", 
// 				*(long *)((long *)data + 1), *(long *)data);
//            else fprintf(target,"%ld%07ld", 
// 				*(long *)((long *)data + 1), *(long *)data);
// #else
// 		fprintf(target, HUGE_FORMAT, *(DSS_HUGE *)data);
// #endif /* SUPPORT_64BITS */
// 		break;
// 	case DT_KEY:
// 		fprintf(target, "%ld", (long)data);
// 		break;
// 	case DT_MONEY:
// 		cents = (long)data;
// 		if (cents < 0)
// 			{
// 			fprintf(target, "-");
// 			cents = -cents;
// 			}
// 		dollars = cents / 100;
// 		cents %= 100;
// 		if (columnar)
// 			fprintf(target, "%12ld.%02ld", dollars, cents);
// 		else
// 			fprintf(target, "%ld.%02ld", dollars, cents);
// 		break;
// 	case DT_CHR:
// 		if (columnar)
// 			fprintf(target, "%c ", (char)data);
// 		else
// 			fprintf(target, "%c", (char)data);
// 		break;
// 	}

// #ifdef EOL_HANDLING
// 	if (sep)
// #endif /* EOL_HANDLING */
// 	if (!columnar && (sep != -1))
// 		fprintf(target, "%c", SEPARATOR);
	
	return(0);
}

#ifdef SSBM
int
pr_cust(customer_t *c, int mode)
{
static FILE *fp = NULL;
        
   if (fp == NULL)
        fp = print_prep(CUST, 0);

   PR_STRT(fp);
   PR_INT(fp, c->custkey);
   PR_VSTR(fp, c->name, C_NAME_LEN);
   PR_VSTR(fp, c->address, 
       (columnar)?(long)(ceil(C_ADDR_LEN * V_STR_HGH)):c->alen);
   PR_STR(fp, c->city,CITY_FIX);
   PR_STR(fp, c->nation_name, C_NATION_NAME_LEN);
   PR_STR(fp, c->region_name, C_REGION_NAME_LEN);
   PR_STR(fp, c->phone, PHONE_LEN);
   PR_STR(fp, c->mktsegment,MAXAGG_LEN);
   PR_END(fp);

   return(0);
}

#else

/*
customer_columns = [
    "C_CUSTKEY",        # identifier
    "C_NAME",           # fixed text, size 25
    "C_ADDRESS",        # variable text, size 40
    "C_NATIONKEY",      # identifier
    "C_PHONE",          # fixed text, size 15
    "C_ACCTBAL",        # decimal
    "C_MKTSEGMENT",     # fixed text, size 10
    "C_COMMENT"         # variable text, size 117
]
*/


INT_t *C_CUSTKEY;
STR_t *C_NAME;
STR_t *C_ADDRESS;
INT_t *C_NATIONKEY;
STR_t *C_PHONE;
FLOAT_t *C_ACCTBAL;
STR_t *C_MKTSEGMENT;
STR_t *C_COMMENT;
uint64_t rows_C = 0, cnt_C = 0;

void pre_pr_cust(uint64_t rows) {
    fprintf(stderr, "pre_pr_cust: Allocating memory for %lu rows\n", rows);
    // Allocate memory for customer data
    C_CUSTKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    C_NAME = (STR_t *)malloc(rows * 25 * sizeof(STR_t));
    C_ADDRESS = (STR_t *)malloc(rows * 40 * sizeof(STR_t));
    C_NATIONKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    C_PHONE = (STR_t *)malloc(rows * 15 * sizeof(STR_t));
    C_ACCTBAL = (FLOAT_t *)malloc(rows * sizeof(FLOAT_t));
    C_MKTSEGMENT = (STR_t *)malloc(rows * 10 * sizeof(STR_t));
    C_COMMENT = (STR_t *)malloc(rows * 117 * sizeof(STR_t));
}


void post_pr_cust() {
    int64_t rows = cnt_C;
    fprintf(stderr, "post_pr_cust: saving pth.\n");
    
    torch::Tensor tensor = torch::from_blob(C_CUSTKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "C_CUSTKEY.pth");
    fprintf(stderr, "post_pr_cust: saved C_CUSTKEY.pth\n");

    tensor = torch::from_blob(C_NAME, {rows, 25}, TQP_STR_TYPE);
    torch::save(tensor, "C_NAME.pth");
    fprintf(stderr, "post_pr_cust: saved C_NAME.pth\n");

    tensor = torch::from_blob(C_ADDRESS, {rows, 40}, TQP_STR_TYPE);
    torch::save(tensor, "C_ADDRESS.pth");
    fprintf(stderr, "post_pr_cust: saved C_ADDRESS.pth\n");

    tensor = torch::from_blob(C_NATIONKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "C_NATIONKEY.pth");
    fprintf(stderr, "post_pr_cust: saved C_NATIONKEY.pth\n");

    tensor = torch::from_blob(C_PHONE, {rows, 15}, TQP_STR_TYPE);
    torch::save(tensor, "C_PHONE.pth");
    fprintf(stderr, "post_pr_cust: saved C_PHONE.pth\n");

    tensor = torch::from_blob(C_ACCTBAL, {rows}, TQP_FLOAT_TYPE);
    torch::save(tensor, "C_ACCTBAL.pth");
    fprintf(stderr, "post_pr_cust: saved C_ACCTBAL.pth\n");

    tensor = torch::from_blob(C_MKTSEGMENT, {rows, 10}, TQP_STR_TYPE);
    torch::save(tensor, "C_MKTSEGMENT.pth");
    fprintf(stderr, "post_pr_cust: saved C_MKTSEGMENT.pth\n");

    tensor = torch::from_blob(C_COMMENT, {rows, 117}, TQP_STR_TYPE);
    torch::save(tensor, "C_COMMENT.pth");
    fprintf(stderr, "post_pr_cust: saved C_COMMENT.pth\n");

    free(C_CUSTKEY);
    free(C_NAME);
    free(C_ADDRESS);
    free(C_NATIONKEY);
    free(C_PHONE);
    free(C_ACCTBAL);
    free(C_MKTSEGMENT);
    free(C_COMMENT);
}

int pr_cust(customer_t *c, int mode)
{
// static FILE *fp = NULL;
        
//    if (fp == NULL)
//         fp = print_prep(CUST, 0);

//    PR_STRT(fp);
//    PR_INT(fp, c->custkey);
//    PR_VSTR(fp, c->name, C_NAME_LEN);
//    PR_VSTR(fp, c->address, 
//        (columnar)?(long)(ceil(C_ADDR_LEN * V_STR_HGH)):c->alen);
//    PR_INT(fp, c->nation_code);
//    PR_STR(fp, c->phone, PHONE_LEN);
//    PR_MONEY(fp, c->acctbal);
//    PR_STR(fp, c->mktsegment, C_MSEG_LEN);
//    PR_VSTR_LAST(fp, c->comment, 
//        (columnar)?(long)(ceil(C_CMNT_LEN * V_STR_HGH)):c->clen);
//    PR_END(fp);
    
    C_CUSTKEY[cnt_C] = c->custkey;
    strncpy((char *)&C_NAME[cnt_C * 25], c->name, 25);
    strncpy((char *)&C_ADDRESS[cnt_C * 40], c->address, 40);
    C_NATIONKEY[cnt_C] = c->nation_code;
    strncpy((char *)&C_PHONE[cnt_C * 15], c->phone, 15);
    C_ACCTBAL[cnt_C] = static_cast<double>(c->acctbal) / 100;
    strncpy((char *)&C_MKTSEGMENT[cnt_C * 10], c->mktsegment, 10);
    strncpy((char *)&C_COMMENT[cnt_C * 117], c->comment, 117);
    
    cnt_C++;
   return(0);
}
#endif

/*
 * print the numbered order 
 */
#ifdef SSBM

#else

/*

orders_columns = [
    "O_ORDERKEY",       # identifier
    "O_CUSTKEY",        # identifier
    "O_ORDERSTATUS",    # fixed text, size 1
    "O_TOTALPRICE",     # decimal
    "O_ORDERDATE",      # date
    "O_ORDERPRIORITY",  # fixed text, size 15
    "O_CLERK",          # fixed text, size 15 (Unused!)
    "O_SHIPPRIORITY",   # integer
    "O_COMMENT"         # variable text, size 79
]

*/

INT_t *O_ORDERKEY;
INT_t *O_CUSTKEY;
STR_t *O_ORDERSTATUS;
FLOAT_t *O_TOTALPRICE;
DATE_t *O_ORDERDATE;
STR_t *O_ORDERPRIORITY;
INT_t *O_SHIPPRIORITY;
STR_t *O_COMMENT;
uint64_t rows_O = 0, cnt_O = 0;

void pre_pr_order(uint64_t rows) {
    fprintf(stderr, "pre_pr_order: Allocating memory for %lu rows\n", rows);
    O_ORDERKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    O_CUSTKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    O_ORDERSTATUS = (STR_t *)malloc(rows * sizeof(STR_t));
    O_TOTALPRICE = (FLOAT_t *)malloc(rows * sizeof(FLOAT_t));
    O_ORDERDATE = (DATE_t *)malloc(rows * sizeof(DATE_t));
    O_ORDERPRIORITY = (STR_t *)malloc(rows * 15 * sizeof(STR_t));
    O_SHIPPRIORITY = (INT_t *)malloc(rows * sizeof(INT_t));
    O_COMMENT = (STR_t *)malloc(rows * 79 * sizeof(STR_t));
}

void post_pr_order() {
    int64_t rows = cnt_O;
    fprintf(stderr, "post_pr_order: saving pth.\n");

    torch::Tensor tensor = torch::from_blob(O_ORDERKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "O_ORDERKEY.pth");
    fprintf(stderr, "post_pr_order: saved O_ORDERKEY.pth\n");
    
    tensor = torch::from_blob(O_CUSTKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "O_CUSTKEY.pth");
    fprintf(stderr, "post_pr_order: saved O_CUSTKEY.pth\n");

    tensor = torch::from_blob(O_ORDERSTATUS, {rows}, TQP_STR_TYPE);
    torch::save(tensor, "O_ORDERSTATUS.pth");
    fprintf(stderr, "post_pr_order: saved O_ORDERSTATUS.pth\n");

    tensor = torch::from_blob(O_TOTALPRICE, {rows}, TQP_FLOAT_TYPE);
    torch::save(tensor, "O_TOTALPRICE.pth");
    fprintf(stderr, "post_pr_order: saved O_TOTALPRICE.pth\n");

    tensor = torch::from_blob(O_ORDERDATE, {rows}, TQP_DATE_TYPE);
    torch::save(tensor, "O_ORDERDATE.pth");
    fprintf(stderr, "post_pr_order: saved O_ORDERDATE.pth\n");

    tensor = torch::from_blob(O_ORDERPRIORITY, {rows, 15}, TQP_STR_TYPE);
    torch::save(tensor, "O_ORDERPRIORITY.pth");
    fprintf(stderr, "post_pr_order: saved O_ORDERPRIORITY.pth\n");

    tensor = torch::from_blob(O_SHIPPRIORITY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "O_SHIPPRIORITY.pth");
    fprintf(stderr, "post_pr_order: saved O_SHIPPRIORITY.pth\n");

    tensor = torch::from_blob(O_COMMENT, {rows, 79}, TQP_STR_TYPE);
    torch::save(tensor, "O_COMMENT.pth");
    fprintf(stderr, "post_pr_order: saved O_COMMENT.pth\n");

    free(O_ORDERKEY);
    free(O_CUSTKEY);
    free(O_ORDERSTATUS);
    free(O_TOTALPRICE);
    free(O_ORDERDATE);
    free(O_ORDERPRIORITY);
    free(O_SHIPPRIORITY);
    free(O_COMMENT);
}

int
pr_order(order_t *o, int mode)
{
    
    // static FILE *fp_o = NULL;
    // static int last_mode = 0;

    // if (fp_o == NULL || mode != last_mode)
    //     {
    //     if (fp_o) 
    //         fclose(fp_o);
    //     fp_o = print_prep(ORDER, mode);
    //     last_mode = mode;
    //     }


    O_ORDERKEY[cnt_O] = *(o->okey);
    O_CUSTKEY[cnt_O] = o->custkey;
    O_ORDERSTATUS[cnt_O] = o->orderstatus;
    O_TOTALPRICE[cnt_O] = static_cast<double>(o->totalprice) / 100;
    O_ORDERDATE[cnt_O] = convert(o->odate);
    strncpy((char *)&O_ORDERPRIORITY[cnt_O * 15], o->opriority, 15);
    O_SHIPPRIORITY[cnt_O] = o->spriority;
    strncpy((char *)&O_COMMENT[cnt_O * 79], o->comment, 79);
    cnt_O++;
    
    // PR_STRT(fp_o);
    // PR_HUGE(fp_o, o->okey);
    // PR_INT(fp_o, o->custkey);
    // PR_CHR(fp_o, o->orderstatus);
    // PR_MONEY(fp_o, o->totalprice);
    // PR_STR(fp_o, o->odate, DATE_LEN);
    // PR_STR(fp_o, o->opriority, O_OPRIO_LEN);
    // PR_STR(fp_o, o->clerk, O_CLRK_LEN);
    // PR_INT(fp_o, o->spriority);
    // PR_VSTR_LAST(fp_o, o->comment, 
    //    (columnar)?(long)(ceil(O_CMNT_LEN * V_STR_HGH)):o->clen);
    // PR_END(fp_o);

    return(0);
}
#endif

/*
 * print an order's lineitems
 */
#ifdef SSBM
int
pr_line(order_t *o, int mode)
{

    static FILE *fp_l = NULL;
    static int last_mode = 0;
    long      i;
    int days;
    char buf[100];

    if (fp_l == NULL || mode != last_mode)
        {
        if (fp_l) 
            fclose(fp_l);
        fp_l = print_prep(LINE, mode);
        last_mode = mode;
        }

    for (i = 0; i < o->lines; i++)
        {
        PR_STRT(fp_l);
        PR_HUGE(fp_l, o->lineorders[i].okey);
        PR_INT(fp_l, o->lineorders[i].linenumber);
	PR_INT(fp_l, o->lineorders[i].custkey);
	PR_INT(fp_l, o->lineorders[i].partkey);
        PR_INT(fp_l, o->lineorders[i].suppkey);
        PR_STR(fp_l, o->lineorders[i].orderdate, DATE_LEN);
	PR_STR(fp_l, o->lineorders[i].opriority, O_OPRIO_LEN);
	PR_INT(fp_l, o->lineorders[i].ship_priority);
        PR_INT(fp_l, o->lineorders[i].quantity);
        PR_INT(fp_l, o->lineorders[i].extended_price);
        PR_INT(fp_l, o->lineorders[i].order_totalprice);
        PR_INT(fp_l, o->lineorders[i].discount);
        PR_INT(fp_l, o->lineorders[i].revenue);
	PR_INT(fp_l, o->lineorders[i].supp_cost);
	PR_INT(fp_l, o->lineorders[i].tax);
	PR_STR(fp_l, o->lineorders[i].commit_date, DATE_LEN);
	PR_STR(fp_l, o->lineorders[i].shipmode, O_SHIP_MODE_LEN);
        PR_END(fp_l);
        }

   return(0);
}
#else

/*

"L_ORDERKEY",         # identifier
"L_PARTKEY",         # identifier
"L_SUPPKEY",         # identifier
"L_QUANTITY",        # integer
"L_EXTENDEDPRICE",   # decimal !
"L_DISCOUNT",        # decimal !
"L_TAX",             # decimal !
"L_RETURNFLAG",      # fixed text, size 1 !
"L_LINESTATUS",      # fixed text, size 1 !
"L_SHIPDATE",        # date !
"L_COMMITDATE",      # date
"L_RECEIPTDATE",     # date
"L_SHIPINSTRUCT",    # fixed text, size 25 !
"L_SHIPMODE",        # fixed text, size 10

 */

INT_t *L_ORDERKEY;
INT_t *L_PARTKEY;
INT_t *L_SUPPKEY;
INT_t *L_QUANTITY;
FLOAT_t *L_EXTENDEDPRICE;
FLOAT_t *L_DISCOUNT;
FLOAT_t *L_TAX;
STR_t *L_RETURNFLAG;
STR_t *L_LINESTATUS;
DATE_t *L_SHIPDATE;
DATE_t *L_COMMITDATE;
DATE_t *L_RECEIPTDATE;
STR_t *L_SHIPINSTRUCT;
STR_t *L_SHIPMODE;
int64_t rows_L = 0, cnt_L = 0;

void pre_pr_line(uint64_t rows) {
    rows *= 5; // upperbound approximation
    fprintf(stderr, "pre_pr_line: Allocating memory for %lu rows\n", rows);
    L_ORDERKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    L_PARTKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    L_SUPPKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    L_QUANTITY = (INT_t *)malloc(rows * sizeof(INT_t));
    L_EXTENDEDPRICE = (FLOAT_t *)malloc(rows * sizeof(FLOAT_t));
    L_DISCOUNT = (FLOAT_t *)malloc(rows * sizeof(FLOAT_t));
    L_TAX = (FLOAT_t *)malloc(rows * sizeof(FLOAT_t));
    L_RETURNFLAG = (STR_t *)malloc(rows * sizeof(STR_t));
    L_LINESTATUS = (STR_t *)malloc(rows * sizeof(STR_t));
    L_SHIPDATE = (DATE_t *)malloc(rows * sizeof(DATE_t));
    L_COMMITDATE = (DATE_t *)malloc(rows * sizeof(DATE_t));
    L_RECEIPTDATE = (DATE_t *)malloc(rows * sizeof(DATE_t));
    L_SHIPINSTRUCT = (STR_t *)malloc(rows * 25 * sizeof(STR_t));
    L_SHIPMODE = (STR_t *)malloc(rows * 10 * sizeof(STR_t));
}

void post_pr_line() {
    int64_t rows = cnt_L;
    fprintf(stderr, "post_pr_line: saving pth.\n");

    torch::Tensor tensor = torch::from_blob(L_ORDERKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "L_ORDERKEY.pth");
    fprintf(stderr, "post_pr_line: saved L_ORDERKEY.pth\n");
    
    tensor = torch::from_blob(L_PARTKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "L_PARTKEY.pth");
    fprintf(stderr, "post_pr_line: saved L_PARTKEY.pth\n");

    tensor = torch::from_blob(L_SUPPKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "L_SUPPKEY.pth");
    fprintf(stderr, "post_pr_line: saved L_SUPPKEY.pth\n");

    tensor = torch::from_blob(L_QUANTITY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "L_QUANTITY.pth");
    fprintf(stderr, "post_pr_line: saved L_QUANTITY.pth\n");

    tensor = torch::from_blob(L_EXTENDEDPRICE, {rows}, TQP_FLOAT_TYPE);
    torch::save(tensor, "L_EXTENDEDPRICE.pth");
    fprintf(stderr, "post_pr_line: saved L_EXTENDEDPRICE.pth\n");

    tensor = torch::from_blob(L_DISCOUNT, {rows}, TQP_FLOAT_TYPE);
    torch::save(tensor, "L_DISCOUNT.pth");
    fprintf(stderr, "post_pr_line: saved L_DISCOUNT.pth\n");

    tensor = torch::from_blob(L_TAX, {rows}, TQP_FLOAT_TYPE);
    torch::save(tensor, "L_TAX.pth");
    fprintf(stderr, "post_pr_line: saved L_TAX.pth\n");

    tensor = torch::from_blob(L_RETURNFLAG, {rows}, TQP_STR_TYPE);
    torch::save(tensor, "L_RETURNFLAG.pth");
    fprintf(stderr, "post_pr_line: saved L_RETURNFLAG.pth\n");

    tensor = torch::from_blob(L_LINESTATUS, {rows}, TQP_STR_TYPE);
    torch::save(tensor, "L_LINESTATUS.pth");
    fprintf(stderr, "post_pr_line: saved L_LINESTATUS.pth\n");

    tensor = torch::from_blob(L_SHIPDATE, {rows}, TQP_DATE_TYPE);
    torch::save(tensor, "L_SHIPDATE.pth");
    fprintf(stderr, "post_pr_line: saved L_SHIPDATE.pth\n");

    tensor = torch::from_blob(L_COMMITDATE, {rows}, TQP_DATE_TYPE);
    torch::save(tensor, "L_COMMITDATE.pth");
    fprintf(stderr, "post_pr_line: saved L_COMMITDATE.pth\n");

    tensor = torch::from_blob(L_RECEIPTDATE, {rows}, TQP_DATE_TYPE);
    torch::save(tensor, "L_RECEIPTDATE.pth");
    fprintf(stderr, "post_pr_line: saved L_RECEIPTDATE.pth\n");

    tensor = torch::from_blob(L_SHIPINSTRUCT, {rows, 25}, TQP_STR_TYPE);
    torch::save(tensor, "L_SHIPINSTRUCT.pth");
    fprintf(stderr, "post_pr_line: saved L_SHIPINSTRUCT.pth\n");

    tensor = torch::from_blob(L_SHIPMODE, {rows, 10}, TQP_STR_TYPE);
    torch::save(tensor, "L_SHIPMODE.pth");
    fprintf(stderr, "post_pr_line: saved L_SHIPMODE.pth\n");

    free(L_ORDERKEY);
    free(L_PARTKEY);
    free(L_SUPPKEY);
    free(L_QUANTITY);
    free(L_EXTENDEDPRICE);
    free(L_DISCOUNT);
    free(L_TAX);
    free(L_RETURNFLAG);
    free(L_LINESTATUS);
    free(L_SHIPDATE);
    free(L_COMMITDATE);
    free(L_RECEIPTDATE);
    free(L_SHIPINSTRUCT);
    free(L_SHIPMODE);
}

int pr_line(order_t *o, int mode)
{
    // static FILE *fp_l = NULL;
    // static int last_mode = 0;
    // long      i;
    // int days;
    // char buf[100];

    // if (fp_l == NULL || mode != last_mode)
    //     {
    //     if (fp_l) 
    //         fclose(fp_l);
    //     fp_l = print_prep(LINE, mode);
    //     last_mode = mode;
    //     }
    // fprintf(stderr, "o_>lines = %ld\n", o->lines);
    for (int i = 0; i < o->lines; i++)
        {
        // fprintf(stderr, "okey = %d %d %d %d\n", *(o->l[i].okey), *(o->l[i].okey+1), *(o->l[i].okey+2), *(o->l[i].okey+3));
        L_ORDERKEY[cnt_L] = *(o->l[i].okey);
        L_PARTKEY[cnt_L] = o->l[i].partkey;
        L_SUPPKEY[cnt_L] = o->l[i].suppkey;
        L_QUANTITY[cnt_L] = o->l[i].quantity;
        L_EXTENDEDPRICE[cnt_L] = static_cast<double>(o->l[i].eprice) / 100;
        L_DISCOUNT[cnt_L] = static_cast<double>(o->l[i].discount) / 100;
        L_TAX[cnt_L] = static_cast<double>(o->l[i].tax) / 100;
        L_RETURNFLAG[cnt_L] = o->l[i].rflag[0];
        L_LINESTATUS[cnt_L] = o->l[i].lstatus[0];
        L_SHIPDATE[cnt_L] = convert(o->l[i].sdate);
        L_COMMITDATE[cnt_L] = convert(o->l[i].cdate);
        L_RECEIPTDATE[cnt_L] = convert(o->l[i].rdate);
        strncpy((char *)&L_SHIPINSTRUCT[cnt_L * 25], o->l[i].shipinstruct, 25);
        strncpy((char *)&L_SHIPMODE[cnt_L * 10], o->l[i].shipmode, 10);
        cnt_L++;
        // PR_STRT(fp_l);
        // PR_HUGE(fp_l, o->l[i].okey);
        
        // PR_INT(fp_l, o->l[i].partkey);
        // PR_INT(fp_l, o->l[i].suppkey);
        // PR_INT(fp_l, o->l[i].lcnt);
        // PR_INT(fp_l, o->l[i].quantity);
        // PR_MONEY(fp_l, o->l[i].eprice);
        // PR_MONEY(fp_l, o->l[i].discount);
        // PR_MONEY(fp_l, o->l[i].tax);
        // PR_CHR(fp_l, o->l[i].rflag[0]);
        // PR_CHR(fp_l, o->l[i].lstatus[0]);
        // PR_STR(fp_l, o->l[i].sdate, DATE_LEN);
        // PR_STR(fp_l, o->l[i].cdate, DATE_LEN);
        // PR_STR(fp_l, o->l[i].rdate, DATE_LEN);
        // PR_STR(fp_l, o->l[i].shipinstruct, L_INST_LEN);
        // PR_STR(fp_l, o->l[i].shipmode, L_SMODE_LEN);
        // PR_VSTR_LAST(fp_l, o->l[i].comment, 
        //     (columnar)?(long)(ceil(L_CMNT_LEN *
        // V_STR_HGH)):o->l[i].clen);
        // PR_END(fp_l);
        }

   return(0);
}
#endif

/*
 * print the numbered order *and* its associated lineitems
 */
#ifdef SSBM
#else
int
pr_order_line(order_t *o, int mode)
{
    tdefs[ORDER].name = tdefs[ORDER_LINE].name;
    pr_order(o, mode);
    pr_line(o, mode);

    return(0);
}
#endif

/*
 * print the given part
 */
#ifdef SSBM
int
pr_part(part_t *part, int mode)
{
    static FILE *p_fp = NULL;

    if (p_fp == NULL)
	p_fp = print_prep(PART, 0);

    PR_STRT(p_fp);
    PR_INT(p_fp, part->partkey);
    PR_VSTR(p_fp, part->name,
            (columnar)?(long)P_NAME_LEN:part->nlen);
    PR_STR(p_fp, part->mfgr, P_MFG_LEN);
    PR_STR(p_fp, part->category, P_CAT_LEN);
    PR_STR(p_fp, part->brand, P_BRND_LEN);

    /*need to handle color*/
    PR_VSTR(p_fp, part->color,(columnar)?(long)P_COLOR_LEN:part->clen);
    PR_VSTR(p_fp, part->type,
	    (columnar)?(long)P_TYPE_LEN:part->tlen);
    PR_INT(p_fp, part->size);
    PR_STR(p_fp, part->container, P_CNTR_LEN);
    PR_END(p_fp);
    return(0);
}

#else

/*
part_columns = [
    "P_PARTKEY",        # identifier
    "P_NAME",           # variable text, size 55
    "P_MFGR",           # fixed text, size 25
    "P_BRAND",          # fixed text, size 10
    "P_TYPE",           # variable text, size 25
    "P_SIZE",           # integer
    "P_CONTAINER",      # fixed text, size 10
    "P_RETAILPRICE",    # decimal
    "P_COMMENT"         # variable text, size 23 (NO.)
]
*/

INT_t *P_PARTKEY;
STR_t *P_NAME;
STR_t *P_MFGR;
STR_t *P_BRAND;
STR_t *P_TYPE;
INT_t *P_SIZE_;
STR_t *P_CONTAINER;
FLOAT_t *P_RETAILPRICE;
int64_t cnt_P = 0;

void pre_pr_part(uint64_t rows) {
    fprintf(stderr, "pre_pr_part: Allocating memory for %lu rows\n", rows);
    P_PARTKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    P_NAME = (STR_t *)malloc(rows * 55 * sizeof(STR_t));
    P_MFGR = (STR_t *)malloc(rows * 25 * sizeof(STR_t));
    P_BRAND = (STR_t *)malloc(rows * 10 * sizeof(STR_t));
    P_TYPE = (STR_t *)malloc(rows * 25 * sizeof(STR_t));
    P_SIZE_ = (INT_t *)malloc(rows * sizeof(INT_t));
    P_CONTAINER = (STR_t *)malloc(rows * 10 * sizeof(STR_t));
    P_RETAILPRICE = (FLOAT_t *)malloc(rows * sizeof(FLOAT_t));
    fprintf(stderr, "pre_pr_part: Finish.\n");
}
void post_pr_part() {
    int64_t rows = cnt_P;
    fprintf(stderr, "post_pr_part: saving pth.\n");

    torch::Tensor tensor = torch::from_blob(P_PARTKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "P_PARTKEY.pth");
    fprintf(stderr, "post_pr_part: saved P_PARTKEY.pth\n");

    tensor = torch::from_blob(P_NAME, {rows, 55}, TQP_STR_TYPE);
    torch::save(tensor, "P_NAME.pth");
    fprintf(stderr, "post_pr_part: saved P_NAME.pth\n");

    tensor = torch::from_blob(P_MFGR, {rows, 25}, TQP_STR_TYPE);
    torch::save(tensor, "P_MFGR.pth");
    fprintf(stderr, "post_pr_part: saved P_MFGR.pth\n");

    tensor = torch::from_blob(P_BRAND, {rows, 10}, TQP_STR_TYPE);
    torch::save(tensor, "P_BRAND.pth");
    fprintf(stderr, "post_pr_part: saved P_BRAND.pth\n");

    tensor = torch::from_blob(P_TYPE, {rows, 25}, TQP_STR_TYPE);
    torch::save(tensor, "P_TYPE.pth");
    fprintf(stderr, "post_pr_part: saved P_TYPE.pth\n");

    tensor = torch::from_blob(P_SIZE_, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "P_SIZE.pth");
    fprintf(stderr, "post_pr_part: saved P_SIZE.pth\n");

    tensor = torch::from_blob(P_CONTAINER, {rows, 10}, TQP_STR_TYPE);
    torch::save(tensor, "P_CONTAINER.pth");
    fprintf(stderr, "post_pr_part: saved P_CONTAINER.pth\n");

    tensor = torch::from_blob(P_RETAILPRICE, {rows}, TQP_FLOAT_TYPE);
    torch::save(tensor, "P_RETAILPRICE.pth");
    fprintf(stderr, "post_pr_part: saved P_RETAILPRICE.pth\n");

    free(P_PARTKEY);
    free(P_NAME);
    free(P_MFGR);
    free(P_BRAND);
    free(P_TYPE);
    free(P_SIZE_);
    free(P_CONTAINER);
    free(P_RETAILPRICE);
}

int
pr_part(part_t *part, int mode)
{
// static FILE *p_fp = NULL;

//     if (p_fp == NULL)
//         p_fp = print_prep(PART, 0);

//    PR_STRT(p_fp);
//    PR_INT(p_fp, part->partkey);
//    PR_VSTR(p_fp, part->name,
//        (columnar)?(long)P_NAME_LEN:part->nlen);
//    PR_STR(p_fp, part->mfgr, P_MFG_LEN);
//    PR_STR(p_fp, part->brand, P_BRND_LEN);
//    PR_VSTR(p_fp, part->type,
//        (columnar)?(long)P_TYPE_LEN:part->tlen);
//    PR_INT(p_fp, part->size);
//    PR_STR(p_fp, part->container, P_CNTR_LEN);
//    PR_MONEY(p_fp, part->retailprice);
//    PR_VSTR_LAST(p_fp, part->comment, 
//        (columnar)?(long)(ceil(P_CMNT_LEN * V_STR_HGH)):part->clen);
//    PR_END(p_fp);
    
    P_PARTKEY[cnt_P] = part->partkey;
    strncpy((char *)&P_NAME[cnt_P * 55], part->name, 55);
    strncpy((char *)&P_MFGR[cnt_P * 25], part->mfgr, 25);
    strncpy((char *)&P_BRAND[cnt_P * 10], part->brand, 10);
    strncpy((char *)&P_TYPE[cnt_P * 25], part->type, 25);
    P_SIZE_[cnt_P] = part->size;
    strncpy((char *)&P_CONTAINER[cnt_P * 10], part->container, 10);
    P_RETAILPRICE[cnt_P] = static_cast<double>(part->retailprice) / 100;
    cnt_P++;
   return(0);
}
#endif

/*
 * print the given part's suppliers
 */
#ifdef SSBM
/*SSBM don't have partsupplier table*/       
#else

/*
partsupp_columns = [
    "PS_PARTKEY",       # identifier
    "PS_SUPPKEY",       # identifier
    "PS_AVAILQTY",      # integer
    "PS_SUPPLYCOST",    # decimal
    "PS_COMMENT"        # variable text, size 199 (NO)
]
*/

INT_t *PS_PARTKEY;
INT_t *PS_SUPPKEY;
INT_t *PS_AVAILQTY;
FLOAT_t *PS_SUPPLYCOST;
int64_t cnt_PS = 0;
void pre_pr_psupp(uint64_t rows) {
    fprintf(stderr, "pre_pr_psupp: Allocating memory for %lu rows\n", rows);
    PS_PARTKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    PS_SUPPKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    PS_AVAILQTY = (INT_t *)malloc(rows * sizeof(INT_t));
    PS_SUPPLYCOST = (FLOAT_t *)malloc(rows * sizeof(FLOAT_t));
    fprintf(stderr, "pre_pr_psupp: Finish.\n");
}
void post_pr_psupp() {
    int64_t rows = cnt_PS;
    fprintf(stderr, "post_pr_psupp: saving pth.\n");

    torch::Tensor tensor = torch::from_blob(PS_PARTKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "PS_PARTKEY.pth");
    fprintf(stderr, "post_pr_psupp: saved PS_PARTKEY.pth\n");

    tensor = torch::from_blob(PS_SUPPKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "PS_SUPPKEY.pth");
    fprintf(stderr, "post_pr_psupp: saved PS_SUPPKEY.pth\n");

    tensor = torch::from_blob(PS_AVAILQTY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "PS_AVAILQTY.pth");
    fprintf(stderr, "post_pr_psupp: saved PS_AVAILQTY.pth\n");

    tensor = torch::from_blob(PS_SUPPLYCOST, {rows}, TQP_FLOAT_TYPE);
    torch::save(tensor, "PS_SUPPLYCOST.pth");
    fprintf(stderr, "post_pr_psupp: saved PS_SUPPLYCOST.pth\n");

    free(PS_PARTKEY);
    free(PS_SUPPKEY);
    free(PS_AVAILQTY);
    free(PS_SUPPLYCOST);
}
int pr_psupp(part_t *part, int mode)
{
    // static FILE *ps_fp = NULL;
    // long      i;

    // if (ps_fp == NULL)
    //     ps_fp = print_prep(PSUPP, mode);

   for (int i = 0; i < SUPP_PER_PART; i++)
      {
    //   PR_STRT(ps_fp);
    //   PR_INT(ps_fp, part->s[i].partkey);
    //   PR_INT(ps_fp, part->s[i].suppkey);
    //   PR_INT(ps_fp, part->s[i].qty);
    //   PR_MONEY(ps_fp, part->s[i].scost);
    //   PR_VSTR_LAST(ps_fp, part->s[i].comment, 
    //    (columnar)?(long)(ceil(PS_CMNT_LEN * V_STR_HGH)):part->s[i].clen);
    //   PR_END(ps_fp);
        PS_PARTKEY[cnt_PS] = part->s[i].partkey;
        PS_SUPPKEY[cnt_PS] = part->s[i].suppkey;
        PS_AVAILQTY[cnt_PS] = part->s[i].qty;
        PS_SUPPLYCOST[cnt_PS] = static_cast<double>(part->s[i].scost) / 100;
        cnt_PS++;
      }

   return(0);
}
#endif

/*
 * print the given part *and* its suppliers
 */
#ifdef SSBM
/*SSBM don't have partsupplier table*/       
#else
int
pr_part_psupp(part_t *part, int mode)
{
    tdefs[PART].name = tdefs[PART_PSUPP].name;
    pr_part(part, mode);
    pr_psupp(part, mode);

    return(0);
}
#endif


#ifdef SSBM
int
pr_supp(supplier_t *supp, int mode)
{
    static FILE *fp = NULL;

    if (fp == NULL)
        fp = print_prep(SUPP, mode);

    PR_STRT(fp);
    PR_INT(fp, supp->suppkey);
    PR_STR(fp, supp->name, S_NAME_LEN);
    
    PR_VSTR(fp, supp->address,
	    (columnar)?(long)(ceil(S_ADDR_LEN * V_STR_HGH)):supp->alen);
    PR_STR(fp, supp->city, CITY_FIX);
    PR_STR(fp, supp->nation_name, C_NATION_NAME_LEN);
    PR_STR(fp, supp->region_name, C_REGION_NAME_LEN);
    PR_STR(fp, supp->phone, PHONE_LEN);
    PR_END(fp);

    return(0);
}
#else

/*
supplier_columns = [
    "S_SUPPKEY",        # identifier
    "S_NAME",           # fixed text, size 25
    "S_ADDRESS",        # variable text, size 40
    "S_NATIONKEY",      # identifier
    "S_PHONE",          # fixed text, size 15
    "S_ACCTBAL",        # decimal
    "S_COMMENT"         # variable text, size 101
]
*/

INT_t *S_SUPPKEY;
STR_t *S_NAME;
STR_t *S_ADDRESS;
INT_t *S_NATIONKEY;
STR_t *S_PHONE;
FLOAT_t *S_ACCTBAL;
STR_t *S_COMMENT;
int64_t cnt_S = 0;

void pre_pr_supp(uint64_t rows) {
    fprintf(stderr, "pre_pr_supp: Allocating memory for %lu rows\n", rows);
    S_SUPPKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    S_NAME = (STR_t *)malloc(rows * 25 * sizeof(STR_t));
    S_ADDRESS = (STR_t *)malloc(rows * 40 * sizeof(STR_t));
    S_NATIONKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    S_PHONE = (STR_t *)malloc(rows * 15 * sizeof(STR_t));
    S_ACCTBAL = (FLOAT_t *)malloc(rows * sizeof(FLOAT_t));
    S_COMMENT = (STR_t *)malloc(rows * 101 * sizeof(STR_t));
    fprintf(stderr, "pre_pr_supp: Finish.\n");
}
void post_pr_supp() {
    int64_t rows = cnt_S;
    fprintf(stderr, "post_pr_supp: saving pth.\n");

    torch::Tensor tensor = torch::from_blob(S_SUPPKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "S_SUPPKEY.pth");
    fprintf(stderr, "post_pr_supp: saved S_SUPPKEY.pth\n");

    tensor = torch::from_blob(S_NAME, {rows, 25}, TQP_STR_TYPE);
    torch::save(tensor, "S_NAME.pth");
    fprintf(stderr, "post_pr_supp: saved S_NAME.pth\n");

    tensor = torch::from_blob(S_ADDRESS, {rows, 40}, TQP_STR_TYPE);
    torch::save(tensor, "S_ADDRESS.pth");
    fprintf(stderr, "post_pr_supp: saved S_ADDRESS.pth\n");

    tensor = torch::from_blob(S_NATIONKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "S_NATIONKEY.pth");
    fprintf(stderr, "post_pr_supp: saved S_NATIONKEY.pth\n");

    tensor = torch::from_blob(S_PHONE, {rows, 15}, TQP_STR_TYPE);
    torch::save(tensor, "S_PHONE.pth");
    fprintf(stderr, "post_pr_supp: saved S_PHONE.pth\n");

    tensor = torch::from_blob(S_ACCTBAL, {rows}, TQP_FLOAT_TYPE);
    torch::save(tensor, "S_ACCTBAL.pth");
    fprintf(stderr, "post_pr_supp: saved S_ACCTBAL.pth\n");

    tensor = torch::from_blob(S_COMMENT, {rows, 101}, TQP_STR_TYPE);
    torch::save(tensor, "S_COMMENT.pth");
    fprintf(stderr, "post_pr_supp: saved S_COMMENT.pth\n");

    free(S_SUPPKEY);
    free(S_NAME);
    free(S_ADDRESS);
    free(S_NATIONKEY);
    free(S_PHONE);
    free(S_ACCTBAL);
    free(S_COMMENT);
}

int
pr_supp(supplier_t *supp, int mode) ////
{
// static FILE *fp = NULL;
        
//    if (fp == NULL)
//         fp = print_prep(SUPP, mode);

//    PR_STRT(fp);
//    PR_INT(fp, supp->suppkey);
//    PR_STR(fp, supp->name, S_NAME_LEN);
//    PR_VSTR(fp, supp->address, 
//        (columnar)?(long)(ceil(S_ADDR_LEN * V_STR_HGH)):supp->alen);
//    PR_INT(fp, supp->nation_code);
//    PR_STR(fp, supp->phone, PHONE_LEN);
//    PR_MONEY(fp, supp->acctbal);
//    PR_VSTR_LAST(fp, supp->comment, 
//        (columnar)?(long)(ceil(S_CMNT_LEN * V_STR_HGH)):supp->clen);
//    PR_END(fp);
    
    S_SUPPKEY[cnt_S] = supp->suppkey;
    strncpy((char *)&S_NAME[cnt_S * 25], supp->name, 25);
    strncpy((char *)&S_ADDRESS[cnt_S * 40], supp->address, 40);
    S_NATIONKEY[cnt_S] = supp->nation_code;
    strncpy((char *)&S_PHONE[cnt_S * 15], supp->phone, 15);
    S_ACCTBAL[cnt_S] = static_cast<double>(supp->acctbal) / 100;
    strncpy((char *)&S_COMMENT[cnt_S * 101], supp->comment, 101);
    cnt_S++;
   return(0);
}
#endif

#ifdef SSBM
#else

/*
nation_columns = [
    "N_NATIONKEY",      # identifier
    "N_NAME",           # fixed text, size 25
    "N_REGIONKEY",      # identifier
    "N_COMMENT"         # variable text, size 152 (no)
]
*/

INT_t *N_NATIONKEY;
STR_t *N_NAME;
INT_t *N_REGIONKEY;
int64_t cnt_N = 0;
void pre_pr_nation(uint64_t rows) {
    fprintf(stderr, "pre_pr_nation: Allocating memory for %lu rows\n", rows);
    N_NATIONKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    N_NAME = (STR_t *)malloc(rows * 25 * sizeof(STR_t));
    N_REGIONKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    fprintf(stderr, "pre_pr_nation: Finish.\n");
}
void post_pr_nation() {
    int64_t rows = cnt_N;
    fprintf(stderr, "post_pr_nation: saving pth.\n");
    torch::Tensor tensor = torch::from_blob(N_NATIONKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "N_NATIONKEY.pth");
    fprintf(stderr, "post_pr_nation: saved N_NATIONKEY.pth\n");
    tensor = torch::from_blob(N_NAME, {rows, 25}, TQP_STR_TYPE);
    torch::save(tensor, "N_NAME.pth");
    fprintf(stderr, "post_pr_nation: saved N_NAME.pth\n");
    tensor = torch::from_blob(N_REGIONKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "N_REGIONKEY.pth");
    fprintf(stderr, "post_pr_nation: saved N_REGIONKEY.pth\n");
    free(N_NATIONKEY);
    free(N_NAME);
    free(N_REGIONKEY);
}  
int
pr_nation(code_t *c, int mode)
{
// static FILE *fp = NULL;
        
//    if (fp == NULL)
//         fp = print_prep(NATION, mode);

//    PR_STRT(fp);
//    PR_INT(fp, c->code);
//    PR_STR(fp, c->text, NATION_LEN);
//    PR_INT(fp, c->join);
//    PR_VSTR_LAST(fp, c->comment, 
//        (columnar)?(long)(ceil(N_CMNT_LEN * V_STR_HGH)):c->clen);
//    PR_END(fp);
    
    N_NATIONKEY[cnt_N] = c->code;
    strncpy((char *)&N_NAME[cnt_N * 25], c->text, 25);
    N_REGIONKEY[cnt_N] = c->join;
    cnt_N++;
   return(0);
}

/*
region_columns = [
    "R_REGIONKEY",      # identifier
    "R_NAME",           # fixed text, size 25
    "R_COMMENT"         # variable text, size 152 (no)
]
*/

INT_t *R_REGIONKEY;
STR_t *R_NAME;
INT_t cnt_R = 0;
void pre_pr_region(uint64_t rows) {
    fprintf(stderr, "pre_pr_region: Allocating memory for %lu rows\n", rows);
    R_REGIONKEY = (INT_t *)malloc(rows * sizeof(INT_t));
    R_NAME = (STR_t *)malloc(rows * 25 * sizeof(STR_t));
    fprintf(stderr, "pre_pr_region: Finish.\n");
}
void post_pr_region() {
    int64_t rows = cnt_R;
    fprintf(stderr, "post_pr_region: saving pth.\n");
    torch::Tensor tensor = torch::from_blob(R_REGIONKEY, {rows}, TQP_INT_TYPE);
    torch::save(tensor, "R_REGIONKEY.pth");
    fprintf(stderr, "post_pr_region: saved R_REGIONKEY.pth\n");
    tensor = torch::from_blob(R_NAME, {rows, 25}, TQP_STR_TYPE);
    torch::save(tensor, "R_NAME.pth");
    fprintf(stderr, "post_pr_region: saved R_NAME.pth\n");
    free(R_REGIONKEY);
    free(R_NAME);
}

int
pr_region(code_t *c, int mode)
{
// static FILE *fp = NULL;
        
//    if (fp == NULL)
//         fp = print_prep(REGION, mode);

//    PR_STRT(fp);
//    PR_INT(fp, c->code);
//    PR_STR(fp, c->text, REGION_LEN);
//    PR_VSTR_LAST(fp, c->comment, 
//        (columnar)?(long)(ceil(R_CMNT_LEN * V_STR_HGH)):c->clen);
//    PR_END(fp);
    R_REGIONKEY[cnt_R] = c->code;
    strncpy((char *)&R_NAME[cnt_R * 25], c->text, 25);
    cnt_R++;
   return(0);
}
#endif

/* 
 * NOTE: this routine does NOT use the BCD2_* routines. As a result,
 * it WILL fail if the keys being deleted exceed 32 bits. Since this
 * would require ~660 update iterations, this seems an acceptable
 * oversight
 */
int
pr_drange(int tbl, long min, long cnt, long num)
{
    static int  last_num = 0;
    static FILE *dfp = NULL;
    int child = -1;
    long start, last, new_;

	static int rows_per_segment=0;
	static int rows_this_segment=0;
	static int residual_rows=0;

    if (last_num != num)
        {
        if (dfp)
            fclose(dfp);
        dfp = print_prep(tbl, -num);
        if (dfp == NULL)
            return(-1);
        last_num = num;
		rows_this_segment=0;
        }

    start = MK_SPARSE(min, (num - 1)/ (10000 / refresh));
    last = start - 1;
    for (child=min; cnt > 0; child++, cnt--)
        {
        new_ = MK_SPARSE(child, (num - 1) / (10000 / refresh));
        if (gen_rng == 1 && new_ - last == 1)
            {
            last = new_;
            continue;
            }
	if (gen_sql)
	    {
	    fprintf(dfp, 
		"delete from %s where %s between %ld and %ld;\n",
		    tdefs[ORDER].name, "o_orderkey", start, last);
	    fprintf(dfp, 
		"delete from %s where %s between %ld and %ld;\n",
		    tdefs[LINE].name, "l_orderkey", start, last);
	    fprintf(dfp, "commit work;\n");
	    }
	else 
	    if (gen_rng)
                {
                PR_STRT(dfp);
                PR_INT(dfp, start);
                PR_INT(dfp, last);
                PR_END(dfp);
                }
            else
                {
				if (delete_segments)
					{
					if(rows_per_segment==0)
						{
						rows_per_segment = (cnt / delete_segments);
						residual_rows = (cnt % delete_segments);
						rows_per_segment++;
						}
					if(delete_segment <= residual_rows)
						{
						if((++rows_this_segment) > rows_per_segment)
							{
							fclose(dfp);
							dfp = print_prep(tbl, -num);
							if (dfp == NULL) return(-1);
							last_num = num;
							rows_this_segment=1;
							}
						}
					else
						{
						if((++rows_this_segment) >= rows_per_segment)
							{
							fclose(dfp);
							dfp = print_prep(tbl, -num);
							if (dfp == NULL) return(-1);
							last_num = num;
							rows_this_segment=1;
							}
						}
					}
                PR_STRT(dfp);
                PR_KEY(dfp, new_);
                PR_END(dfp);
                }
	start = new_;
	last = new_;
        }
    if (gen_rng)
	{
	PR_STRT(dfp);
	PR_INT(dfp, start);
	PR_INT(dfp, last);
	PR_END(dfp);
	}
    
    return(0);
}

#ifdef SSBM
int pr_date(date_t *d, int mode){
    static FILE *d_fp = NULL;
    
    if (d_fp == NULL)
	d_fp = print_prep(DATE, 0);

    PR_STRT(d_fp);
    PR_INT(d_fp, d->datekey);
    PR_STR(d_fp, d->date,D_DATE_LEN);
    PR_STR(d_fp, d->dayofweek,D_DAYWEEK_LEN);
    PR_STR(d_fp, d->month,D_MONTH_LEN);
    PR_INT(d_fp, d->year);
    PR_INT(d_fp, d->yearmonthnum);
    PR_STR(d_fp, d->yearmonth,D_YEARMONTH_LEN);
    PR_INT(d_fp, d->daynuminweek);
    PR_INT(d_fp, d->daynuminmonth);
    PR_INT(d_fp, d->daynuminyear);
    PR_INT(d_fp, d->monthnuminyear);
    PR_INT(d_fp, d->weeknuminyear);
    PR_VSTR(d_fp, 
	    d->sellingseason,(columnar)?(long)D_SEASON_LEN:d->slen);
    PR_STR(d_fp,d->lastdayinweekfl,2);
    PR_STR(d_fp,d->lastdayinmonthfl,2);
    PR_STR(d_fp,d->holidayfl,2);
    PR_STR(d_fp,d->weekdayfl,2);

    PR_END(d_fp);
    return(0);

}

#endif
/*
 * verify functions: routines which replace the pr_routines and generate a pseudo checksum 
 * instead of generating the actual contents of the tables. Meant to allow large scale data 
 * validation without requiring a large amount of storage
 */
#ifdef SSBM
int
vrf_cust(customer_t *c, int mode)
{
   VRF_STRT(CUST);
   VRF_INT(CUST, c->custkey);
   VRF_STR(CUST, c->name);
   VRF_STR(CUST, c->address);
   VRF_STR(CUST, c->city);
   VRF_STR(CUST, c->nation_name);
   VRF_STR(CUST, c->region_name);
   VRF_STR(CUST, c->phone);
   VRF_STR(CUST, c->mktsegment);
   VRF_END(CUST);

   return(0);
}

#else
int
vrf_cust(customer_t *c, int mode)
{
   VRF_STRT(CUST);
   VRF_INT(CUST, c->custkey);
   VRF_STR(CUST, c->name);
   VRF_STR(CUST, c->address);
   VRF_INT(CUST, c->nation_code);
   VRF_STR(CUST, c->phone);
   VRF_MONEY(CUST, c->acctbal);
   VRF_STR(CUST, c->mktsegment);
   VRF_STR(CUST, c->comment);
   VRF_END(CUST);

   return(0);
}
#endif

/*
 * print the numbered order 
 */
#ifdef SSBM
#else
int
vrf_order(order_t *o, int mode)
{
    VRF_STRT(ORDER);
    VRF_HUGE(ORDER, o->okey);
    VRF_INT(ORDER, o->custkey);
    VRF_CHR(ORDER, o->orderstatus);
    VRF_MONEY(ORDER, o->totalprice);
    VRF_STR(ORDER, o->odate);
    VRF_STR(ORDER, o->opriority);
    VRF_STR(ORDER, o->clerk);
    VRF_INT(ORDER, o->spriority);
    VRF_STR(ORDER, o->comment);
    VRF_END(ORDER);

    return(0);
}
#endif

/*
 * print an order's lineitems
 */
#ifdef SSBM
int
vrf_line(order_t *o, int mode)
{
    int i;

    for (i = 0; i < o->lines; i++)
        {
	    VRF_STRT(LINE);
	    VRF_HUGE(LINE, o->lineorders[i].okey);
	    VRF_INT(LINE, o->lineorders[i].linenumber);
	    VRF_INT(LINE, o->lineorders[i].custkey);
	    VRF_INT(LINE, o->lineorders[i].partkey);
	    VRF_INT(LINE, o->lineorders[i].suppkey);
	    VRF_STR(LINE, o->lineorders[i].orderdate);
	    VRF_STR(LINE, o->lineorders[i].opriority);
	    VRF_INT(LINE, o->lineorders[i].ship_priority);
	    VRF_INT(LINE, o->lineorders[i].quantity);
	    VRF_INT(LINE, o->lineorders[i].extended_price);
	    VRF_INT(LINE, o->lineorders[i].order_totalprice);
	    VRF_INT(LINE, o->lineorders[i].discount);
	    VRF_INT(LINE, o->lineorders[i].revenue);
	    VRF_INT(LINE, o->lineorders[i].supp_cost);
	    VRF_INT(LINE, o->lineorders[i].tax);
	    VRF_STR(LINE, o->lineorders[i].commit_date);
	    VRF_STR(LINE, o->lineorders[i].shipmode);
	    VRF_END(LINE);
        }

    return(0);
}

#else
int
vrf_line(order_t *o, int mode)
{
	int i;

    for (i = 0; i < o->lines; i++)
        {
        VRF_STRT(LINE);
        VRF_HUGE(LINE, o->l[i].okey);
        VRF_INT(LINE, o->l[i].partkey);
        VRF_INT(LINE, o->l[i].suppkey);
        VRF_INT(LINE, o->l[i].lcnt);
        VRF_INT(LINE, o->l[i].quantity);
        VRF_MONEY(LINE, o->l[i].eprice);
        VRF_MONEY(LINE, o->l[i].discount);
        VRF_MONEY(LINE, o->l[i].tax);
        VRF_CHR(LINE, o->l[i].rflag[0]);
        VRF_CHR(LINE, o->l[i].lstatus[0]);
        VRF_STR(LINE, o->l[i].sdate);
        VRF_STR(LINE, o->l[i].cdate);
        VRF_STR(LINE, o->l[i].rdate);
        VRF_STR(LINE, o->l[i].shipinstruct);
        VRF_STR(LINE, o->l[i].shipmode);
        VRF_STR(LINE, o->l[i].comment);
        VRF_END(LINE);
        }

   return(0);
}
#endif

/*
 * print the numbered order *and* its associated lineitems
 */
#ifdef SSBM
#else
int
vrf_order_line(order_t *o, int mode)
{
    vrf_order(o, mode);
    vrf_line(o, mode);

    return(0);
}
#endif

/*
 * print the given part
 */
#ifdef SSBM
int
vrf_part(part_t *part, int mode)
{

    VRF_STRT(PART);
    VRF_INT(PART, part->partkey);
    VRF_STR(PART, part->name);
    VRF_STR(PART, part->mfgr);
    VRF_STR(PART, part->brand);
    VRF_STR(PART, part->type);
    VRF_INT(PART, part->size);
    VRF_STR(PART, part->container);
    VRF_STR(PART, part->category);
    VRF_END(PART);

    return(0);
}

#else
int
vrf_part(part_t *part, int mode)
{

   VRF_STRT(PART);
   VRF_INT(PART, part->partkey);
   VRF_STR(PART, part->name);
   VRF_STR(PART, part->mfgr);
   VRF_STR(PART, part->brand);
   VRF_STR(PART, part->type);
   VRF_INT(PART, part->size);
   VRF_STR(PART, part->container);
   VRF_MONEY(PART, part->retailprice);
   VRF_STR(PART, part->comment);
   VRF_END(PART);

   return(0);
}
#endif

/*
 * print the given part's suppliers
 */
#ifdef SSBM
#else
int
vrf_psupp(part_t *part, int mode)
{
    long      i;

   for (i = 0; i < SUPP_PER_PART; i++)
      {
      VRF_STRT(PSUPP);
      VRF_INT(PSUPP, part->s[i].partkey);
      VRF_INT(PSUPP, part->s[i].suppkey);
      VRF_INT(PSUPP, part->s[i].qty);
      VRF_MONEY(PSUPP, part->s[i].scost);
      VRF_STR(PSUPP, part->s[i].comment);
      VRF_END(PSUPP);
      }

   return(0);
}
#endif

/*
 * print the given part *and* its suppliers
 */
#ifdef SSBM
#else
int
vrf_part_psupp(part_t *part, int mode)
{
    vrf_part(part, mode);
    vrf_psupp(part, mode);

    return(0);
}
#endif

#ifdef SSBM
int
vrf_supp(supplier_t *supp, int mode)
{
    VRF_STRT(SUPP);
    VRF_INT(SUPP, supp->suppkey);
    VRF_STR(SUPP, supp->name);
    
    VRF_STR(CUST, supp->address);
    VRF_INT(CUST, supp->nation_key);
    VRF_STR(CUST, supp->nation_name);
    VRF_INT(CUST, supp->region_key);
    VRF_STR(CUST, supp->region_name);
    VRF_STR(CUST, supp->phone);
    VRF_END(SUPP);

    return(0);
}

#else
int
vrf_supp(supplier_t *supp, int mode)
{
   VRF_STRT(SUPP);
   VRF_INT(SUPP, supp->suppkey);
   VRF_STR(SUPP, supp->name);
   VRF_STR(SUPP, supp->address);
   VRF_INT(SUPP, supp->nation_code);
   VRF_STR(SUPP, supp->phone);
   VRF_MONEY(SUPP, supp->acctbal);
   VRF_STR(SUPP, supp->comment); 
   VRF_END(SUPP);

   return(0);
}
#endif

#ifdef SSBM
#else
int
vrf_nation(code_t *c, int mode)
{
   VRF_STRT(NATION);
   VRF_INT(NATION, c->code);
   VRF_STR(NATION, c->text);
   VRF_INT(NATION, c->join);
   VRF_STR(NATION, c->comment);
   VRF_END(NATION);

   return(0);
}

int
vrf_region(code_t *c, int mode)
{
   VRF_STRT(REGION);
   VRF_INT(REGION, c->code);
   VRF_STR(REGION, c->text);
   VRF_STR(REGION, c->comment);
   VRF_END(fp);

   return(0);
}
#endif


#ifdef SSBM
int vrf_date(date_t * d, int mode)
{
    VRF_STRT(DATE);
    VRF_INT(DATE, d->datekey);
    VRF_STR(DATE, d->date);
    VRF_STR(DATE, d->dayofweek);
    VRF_STR(DATE, d->month);
    VRF_INT(DATE, d->year);
    VRF_INT(DATE, d->yearmonthnum);
    VRF_STR(DATE, d->yearmonth);
    VRF_INT(DATE, d->daynuminweek);
    VRF_INT(DATE, d->daynuminmonth);
    VRF_INT(DATE, d->daynuminyear);
    VRF_INT(DATE, d->monthnuminyear);
    VRF_INT(DATE, d->weeknuminyear);
    VRF_STR(DATE, d->sellingseason);
    VRF_STR(DATE, d->lastdayinweekfl);
    VRF_STR(DATE, d->lastdayinmonthfl);
    VRF_STR(DATE, d->weekdayfl);
    VRF_END(DATE);
    return(0);

}
#endif

