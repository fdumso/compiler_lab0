int max(int,int);
int maxargsExp(A_exp);
int maxargs(A_stm);
void interp(A_stm);

typedef struct Table_ *table;
typedef struct IntAndTable_ *intAndTable;

struct Table_ {string id; int value; table tail;};
struct IntAndTable_ {int i; table t;};

table Table(string, int, table);
intAndTable IntAndTable(int, table);
table interpStm(A_stm, table);
table update(table, string, int);
int lookup(table, string);
intAndTable interpExp(A_exp, table);
int interpOP(int, A_binop, int);
