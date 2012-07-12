/*
SL - A small and efficient linked list library.
Copyright (C) 2003-2005 Stig Brautaset. All rights reserved.

This file is part of SL.

SL is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

SL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SL; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

/*

=head1 NAME

sl - a small and flexible linked list implementation

=head1 DESCRIPTION

`sl' provides a generic implementation of singly-linked lists and
stacks.

`sl' does not do extra allocations behind the scenes for
placeholder nodes, yet users of the library can define their
node structure almost any way they want. The one important
thing is that the ->next member is the I<first> member of the
structure.

=cut

*/

#include "sl.h"

#include <stdlib.h>		/* for free() */
#include <stddef.h>		/* for NULL */


/*

=pod

Internally the library uses a structure defined as:

  struct sl_node {
    struct sl_node *next;
  };

But you are allowed to use any structure you want, as long as a
pointer to the next structure in the list is the first member of the
structure. (It doesn't have to be called C<next>, by the way). As an
example, you could build a generic container-based list structure like
so:

  struct cons {
    struct cons *cdr;
    void *car;
  };

=cut

*/

struct sl_node {
	struct sl_node *next;
};


/*

=head1 FUNCTIONS

=over

=item void *sl_push(void *root, void *p)

Push C<p> onto the list C<root>. Return the new list.

=cut

*/
void *sl_push(void *root, void *p)
{
	struct sl_node *q = p;

	if (!q)
		return root;

	q->next = root;
	return q;
}


/*

=item void *sl_pop(void *root)

Pop a node from a list. Return the pop'ed item, or NULL if the
list is empty.

B<Note:> this function takes a pointer to a pointer to a node as
its argument. C does not allow C<void **> to be used as a
generic pointer to pointer type. However, since C<void *> is a
generic pointer, it can also point to a pointer to pointer.

=cut

*/
void *sl_pop(void *root)
{
	struct sl_node **pp = root;
	struct sl_node *p = *pp;

	if (!p)
		return NULL;

	*pp = p->next;
	p->next = NULL;

	return p;
}


/*

=item void *sl_unshift(void *root, void *p)

Shift a node onto the `far end' of a list.
This function can be used to append a list to another.
The new list is returned.

=cut

*/
void *sl_unshift(void *root, void *p)
{
	struct sl_node *q;

	if (!p)
		return root;
	if (!root)
		return p;

	q = root;
	while (q && q->next)
		q = q->next;

	q->next = p;
	return root;
}


/*

=item void *sl_shift(void *)

Shift a node from the `far end' of a list.
Returns the item shifted off the list, or NULL if the list is empty.

B<Note:> this function takes a pointer to a pointer to a node as
its argument. C does not allow C<void **> to be used as a
generic pointer to pointer type. However, since C<void *> is a
generic pointer, it can also point to a pointer to pointer.

=cut

*/
void *sl_shift(void *root)
{
	struct sl_node **pp = root;
	struct sl_node *p = *pp;
	struct sl_node *q;

	/* Initial node will only change if there is only one node in
	 * the list. But it is safe to set it to NULL if it is already
	 * NULL.
	 */
	if (!p || !p->next) {
		*pp = NULL;
		return p;
	}

	q = p;
	while (p && p->next) {
		q = p;
		p = p->next;
	}
	q->next = NULL;
	return p;
}


/*

=item void *sl_reverse(void *root)

Returns the reversed list.

=cut

*/
void *sl_reverse(void *root)
{
	struct sl_node *revlist, *p;

	revlist = NULL;
	while ((p = sl_pop(&root)))
		revlist = sl_push(revlist, p);

	return revlist;
}


/*

=item void *sl_map(void *root, int (*func)(void *, void *), void *data)

Map a function, C<func>, to every element in a list.
The C<data> is handed to C<func> along with each node. This
function can be used for a sequential search of a list of nodes.

This function returns NULL on normal operation. If C<func>
returns non-zero, a pointer to the current node will be returned.

=cut

This function is inspired by Kernighan & Pike's `The Practice of
Programming'.

*/
void *sl_map(void *root, int (*func)(void *, void *), void *data)
{
	struct sl_node *p;

	for (p = root; p != NULL; p = p->next)
		if (func(p, data))
			return p;
	return NULL;
}

/*

=item void *sl_filter(void *root, int (*func)(void *, void *), void *data)

C<func> is called once for each node in the list, having the node itself
passed as the first argument; C<data> is passed as the second argument.
If C<func> returns a positive value the current node will be extracted
from the passed-in list and stored in a temporary list. When we get to
the end of the passed-in list, the temporary list is returned.

If C<func> returns a I<negative> value the same happens as when a
positive value is returened but in addition any further traversal of the
passed-in array is terminated and the current temporary list is returned
immediately.

You can return the first 5 elements that matches a certain criteria by
maintaining a counter in C<data> and return 1 until the fifth node is
found, then return -1.

B<Note:> this function takes a pointer to a pointer to a node as
its argument. C does not allow C<void **> to be used as a
generic pointer to pointer type. However, since C<void *> is a
generic pointer, it can also point to a pointer to pointer.

=cut

*/

void *sl_filter(void *root, int (*func)(void *, void *), void *data)
{
	struct sl_node **pp = root;
	struct sl_node *p = *pp;
	struct sl_node *r = NULL, *head = NULL;

	while (p) {
		struct sl_node *q = sl_pop(&p);
		int val = func(q, data);
		if (!val) {
			head = sl_unshift(head, q);
		}
		else {
			r = sl_unshift(r, q);
			if (val < 0) {
				break;
			}
		}
	}
	*pp = sl_unshift(head, p);
	return r;
}

/*

=item void *sl_split(void *root)

Split a list roughly on the middle; return a pointer to the second
half.

=cut

This function is a pre-requisite for mergesort. Thanks to CB
Falconer for this code.

*/
void *sl_split(void *root)
{
	struct sl_node *p, *p1, *p2;

	if (!root)
		return NULL;

	p1 = p2 = p = root;
	do {
		p2 = p1;
		p1 = p1->next;			/* advance 1 */
		p = p->next;
		if (p) p = p->next;		/* advance 2 */
	} while (p);

	p2->next = NULL;
	return p1;
}


/*

=item void *sl_merge(void *p1, void *p2, int (*cmp)(void *, void *))

Merge two sorted lists and keep the list sorted. This function is
the heart of the mergesort routine. Thanks to CB Falconer for
this code.

=cut

*/
void *sl_merge(void *p1, void *p2, int (*cmp)(void *, void *))
{
	struct sl_node *q1 = p1, *q2 = p2;
        struct sl_node n, *root;

        root = &n;
        n.next = root;

        while (q1 && q2) {
                if (0 >= cmp(q1, q2)) {
                        root->next = q1;
                        root = q1;
                        q1 = q1->next;
                }
                else {
                        root->next = q2;
                        root = q2;
                        q2 = q2->next;
                }
        }

        /* At least one list empty now; append the other. */
	root->next = q1 ? q1 : q2;

        /* check for an empty list */
        if (n.next == &n)
		return NULL;

        return n.next;
}


/*

=item void *sl_mergesort(void *root, int (*cmp)(void *, void *))

Return the sorted list.

=cut

Sort a list using mergesort. The algorithm is recursive.
Thanks to CB Falconer for this code.

*/
void *sl_mergesort(void *root, int (*cmp)(void *, void *))
{
	struct sl_node *p;

	/*
	 * Must be at least two nodes in a list before it can be
	 * unordered.
	 */
	p = root;
	if (p && p->next) {
		p = sl_split(root);
		root = sl_merge(sl_mergesort(root, cmp), sl_mergesort(p, cmp), cmp);
	}

	return root;
}


/*

=item int sl_count(void *p)

Returns the number of elements in a list.

=cut

*/
int sl_count(void *p)
{
        struct sl_node *q;
	int n = 0;

	for (q = p; q; q = q->next)
		n++;

	return n;
}


/*

=item void sl_free(void *root, void (*func)(void*))

A macro that just calls sl__free(). This is necessary because
sl_free() is a defined function on OS-X.

=item void sl__free(void *root, void (*func)(void*))

Free a list of nodes. Takes an optional argument, @p func, used to
free the node if it is defined.

=cut

*/
void sl__free(void *root, void (*func)(void*))
{
	struct sl_node *p;

	if (func == NULL)
		func = free;

	while ((p = sl_pop(&root)))
		func(p);
}

/*

=back

=head1 AUTHOR

Stig Brautaset <stig@brautaset.org>

=head1 CREDITS

Thanks to Thomas Stegen of comp.lang.c for suggesting the
C<void*> trick employed in `sl_pop()` and `sl_shift()`.

Thanks to CB Falconer of comp.programming for help on the
sorting code.

Richard Spindler suggested what became the C<sl_filter()> function.

=head1 COPYRIGHT

Copyright (C) 2003,2004,2005 Stig Brautaset

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

=cut

*/
