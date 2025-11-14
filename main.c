#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void test_file(const char *filename, const char *test_name)
{
	int fd;
	char *line;
	int count = 0;

	printf("\n========== %s ==========\n", test_name);
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		printf("ERROR: Cannot open %s\n", filename);
		return;
	}

	while ((line = get_next_line(fd)) != NULL)
	{
		count++;
		printf("Line %d: [%s]\n", count, line);
		free(line);
		if (count > 50) // Safety pour pas spammer
		{
			printf("... (stopped after 50 lines)\n");
			break;
		}
	}
	printf("Total lines: %d\n", count);

	// Test appel après EOF
	line = get_next_line(fd);
	if (line == NULL)
		printf("✓ Returns NULL after EOF\n");
	else
	{
		printf("✗ ERROR: Returned non-NULL after EOF: [%s]\n", line);
		free(line);
	}

	close(fd);
}

void test_invalid_fd()
{
	char *line;

	printf("\n========== TEST FD INVALIDES ==========\n");

	// FD négatif
	line = get_next_line(-1);
	if (line == NULL)
		printf("✓ FD -1 returns NULL\n");
	else
	{
		printf("✗ ERROR: FD -1 returned: [%s]\n", line);
		free(line);
	}

	// FD fermé
	line = get_next_line(999);
	if (line == NULL)
		printf("✓ FD 999 (closed) returns NULL\n");
	else
	{
		printf("✗ ERROR: FD 999 returned: [%s]\n", line);
		free(line);
	}
}

void test_multiple_fd()
{
	int fd1, fd2;
	char *line1, *line2;

	printf("\n========== TEST MULTIPLE FD (alternés) ==========\n");

	fd1 = open("test.txt", O_RDONLY);
	fd2 = open("test.txt", O_RDONLY);

	if (fd1 < 0 || fd2 < 0)
	{
		printf("ERROR: Cannot open files\n");
		return;
	}

	// Alterne entre les deux FD
	for (int i = 0; i < 5; i++)
	{
		line1 = get_next_line(fd1);
		line2 = get_next_line(fd2);

		printf("FD1 line %d: [%s]\n", i+1, line1 ? line1 : "NULL");
		printf("FD2 line %d: [%s]\n", i+1, line2 ? line2 : "NULL");

		if (line1) free(line1);
		if (line2) free(line2);
	}

	close(fd1);
	close(fd2);
}

int main(void)
{
	printf("====================================\n");
	printf("  GET_NEXT_LINE - STRESS TESTS\n");
	printf("  BUFFER_SIZE = %d\n", BUFFER_SIZE);
	printf("====================================\n");

	// Test le fichier principal
	test_file("test.txt", "TEST FICHIER NORMAL");

	// Test FD invalides
	test_invalid_fd();

	// Test multiple FD (si ton code gère pas le bonus, ça va fail)
	test_multiple_fd();

	printf("\n====================================\n");
	printf("  TESTS TERMINÉS\n");
	printf("====================================\n");

	return 0;
}
