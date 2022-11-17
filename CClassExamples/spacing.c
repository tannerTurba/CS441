#include <stdio.h>

/*
 * Tabs vs. spaces is one of the greatest standing arguments in computer
 * science. I argue here for my position.
 */

int
main(int argc, char *argv[])
{
	/*
	 * Use tabs to indent. Each programmer can set his editor to use his
	 * preferred width for each tab.
	 */
	printf("Hello, world!\n");

	/*
	 * Linus Torvalds prefers a tab width of eight characters. He argues
	 * that this is easier for the eye to see, and if you run out
	 * of horizontal space because of this width, then you should
	 * refactor to use a function. I share this opinion.
	 */

	/*
	 * Keep code around 80 characters wide. More than that is difficult to
	 * read and should be refactored to use one or ore functions.
	 */

	/*
	 * The problem is that (adjustable-width) tabs throw off alignment when
	 * adjusted. This use tabs for indentation, but spaces for alignment.
	 * Here is a silly example:
	 */
	for (int i = 0;
	     i < 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1;
	     i++) {}
/* TAB	SSSSS */
}
