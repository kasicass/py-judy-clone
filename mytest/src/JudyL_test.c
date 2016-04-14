#include <Judy.h>
#include <assert.h>

int main()
{
	Word_t i;
	Word_t memused;
	Word_t ret;
	PWord_t value;
	Pvoid_t array = (Pvoid_t) NULL;

	for (i = 0; i < 1000; ++i)
	{
		// JulyL insert
		JLI(value, array, i);
		*value = i;
	}

	// JulyL MemUsed
	JLMU(memused, array);
	printf("mem used: %lu\n", memused);

	for (i = 0; i < 1000; i += 2)
	{
		// JulyL delete
		JLD(ret, array, i);
		assert(ret == 1);
	}

	JLMU(memused, array);
	printf("mem used: %lu\n", memused);

	JLG(value, array, 0);
	if (value) printf("array[0] = %lu\n", value);
	else printf("array[0] deleted\n");

	JLG(value, array, 1);
	if (value) printf("array[1] = %lu\n", *value);
	else printf("array[1] deleted\n");

	JLC(ret, array, 0, 500);
	printf("[0, 500] Index count: %lu\n", ret);
	JLC(ret, array, 0, -1);
	printf("all Index count: %lu\n", ret);

	JLBC(value, array, 10, i);
	printf("10-th element, [%lu] = %lu\n", i, *value);

	JLFA(ret, array);
	assert(array == NULL);
	printf("freed %lu bytes\n", ret);

    return 0;
}
