#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "slp.h"
#include "prog1.h"

#include "main.h"


int max(int a, int b) {
	if (a >= b) {
		return a;
	} else {
		return b;
	}
}

int maxargsExp(A_exp exp) {
	if (exp->kind == A_eseqExp) {
		return maxargs(exp->u.eseq.stm);
	} else if (exp->kind == A_opExp) {
		int leftMax = maxargsExp(exp->u.op.left);
		int rightMax = maxargsExp(exp->u.op.right);
		return max(leftMax, rightMax);
	} else {
		return 0;
	}
}

int maxargs(A_stm prog){
	if (prog->kind == A_compoundStm) {
		A_stm stm1 = prog->u.compound.stm1;
		A_stm stm2 = prog->u.compound.stm2;

		int max1 = maxargs(stm1);
		int max2 = maxargs(stm2);

		if (max1 >= max2) {
			return max1;
		} else {
			return max2;
		}
	} else if (prog->kind == A_assignStm) {
		A_exp exp = prog->u.assign.exp;
		return maxargsExp(exp);
	} else if (prog->kind == A_printStm) {
		A_expList printList = prog->u.print.exps;
		int count = 1;
		while (printList->kind == A_pairExpList) {
			A_exp head = printList->u.pair.head;
			printList = printList->u.pair.tail;
			count++;
		}
		printList = prog->u.print.exps;
		int max = count;
		while (printList->kind == A_pairExpList) {
			A_exp head = printList->u.pair.head;
			int headMax = maxargsExp(head);

			if (headMax > max) {
				max = headMax;
			}
			printList = printList->u.pair.tail;
		}
		A_exp last = printList->u.last;
		int lastMax = maxargsExp(last);
		if (lastMax > max) {
			max = lastMax;
		}
		return max;
	}
	return 0;
}

table Table(string id, int value, table tail) {
  table t = malloc(sizeof(t));
  t->id=id; t->value=value; t->tail=tail;
  return t;
}

intAndTable IntAndTable(int i, table t) {
	intAndTable iat = malloc(sizeof(iat));
	iat->i = i;
	iat->t = t;
	return iat;
}

table update(table old, string id, int value) {
	return Table(id, value, old);
}

int lookup(table t, string key) {
	if (t->id == key) {
		return t->value;
	} else if (t->tail != NULL){
		return lookup(t->tail, key);
	} else {
		printf("[Error] Identifier %s does not exist!\n", key);
	}
}

table interpStm(A_stm s, table t) {
	if (s->kind == A_compoundStm) {
		A_stm stm1 = s->u.compound.stm1;
		A_stm stm2 = s->u.compound.stm2;
		t = interpStm(stm1, t);
		return interpStm(stm2, t);
	} else if (s->kind == A_assignStm) {
		A_exp exp = s->u.assign.exp;
		string id = s->u.assign.id;
		intAndTable iat = interpExp(exp, t);
		t = iat->t;
		int i = iat->i;
		return update(t, id, i);
	} else if (s->kind == A_printStm) {
		A_expList printList = s->u.print.exps;
		while (printList->kind == A_pairExpList) {
			A_exp head = printList->u.pair.head;
			intAndTable iat = interpExp(head, t);
			t = iat->t;
			int i = iat->i;
			printf("%d ", i);
			printList = printList->u.pair.tail;
		}
		A_exp last = printList->u.last;
		intAndTable iat = interpExp(last, t);
		t = iat->t;
		int i = iat->i;
		printf("%d\n", i);

		return t;
	}
}

int interpOP(int left, A_binop oper, int right) {
	if (oper == A_plus) {
		return left+right;
	} else if (oper == A_minus) {
		return left-right;
	} else if (oper == A_times) {
		return left*right;
	} else if (oper == A_div) {
		return left/right;
	}
}

intAndTable interpExp(A_exp e, table t) {
	if (e->kind == A_idExp) {
		int i = lookup(t, e->u.id);
		return IntAndTable(i, t);
	} else if (e->kind == A_numExp) {
		int i = e->u.num;
		return IntAndTable(i, t);
	} else if (e->kind == A_opExp) {
		A_exp left = e->u.op.left;
		A_exp right = e->u.op.right;
		A_binop oper = e->u.op.oper;
		intAndTable iatLeft = interpExp(left, t);
		int iLeft = iatLeft->i;
		table tLeft = iatLeft->t;
		intAndTable iatRight = interpExp(right, tLeft);
		int iRight = iatRight->i;
		table tRight = iatRight->t;
		int result = interpOP(iLeft, oper, iRight);
		return IntAndTable(result, tRight);
	} else if (e->kind == A_eseqExp) {
		A_stm stm = e->u.eseq.stm;
		A_exp exp = e->u.eseq.exp;
		t = interpStm(stm, t);
		return interpExp(exp, t);
	}
	return NULL;
}

void interp(A_stm prog){
	interpStm(prog, NULL);
}

// DO NOT CHANGE!
int main(){
	printf(">> Right Prog Section:\n");
	A_stm rp = right_prog();
	printf("the maximum number of arguments of any print statement is %d\n",maxargs(rp));
	interp(rp);

	printf(">> Error Prog Section:\n");
	A_stm ep = error_prog();
	printf("the maximum number of arguments of any print statement is %d\n",maxargs(ep));
	interp(ep);
	return 0;
}
