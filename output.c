#include "output.h"

void outputKV (char* key, char* value, bool first_kv_of_record, bool first_line) {
	if (OutputJSON) {
		if (first_kv_of_record) {
			if (first_line)
				fputs("{\"", stdout);
			else
				fputs(",{\"", stdout);
		} else
			fputs(",\"", stdout);

		outputJsonString(key);
		fputs("\":\"", stdout);
		outputJsonString(value);
		fputs("\"", stdout);
	} else {
		if (first_kv_of_record && ! first_line)
			fputs("-\n", stdout);
//		breaksToBlank(key);
		breaksToBlank(value);
		printf("%s: %s\n", key, value);
	}
}

void outputRecordEnd () {
	if (OutputJSON)
		fputs("}", stdout);
}

void outputBegin () {
	if (OutputJSON)
		fputs("[", stdout);
}
void outputEnd () {
	if (OutputJSON)
		fputs("]\n", stdout);
}

