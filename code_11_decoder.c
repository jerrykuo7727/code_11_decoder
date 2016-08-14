#include <stdio.h>

#define dash 10

void decode(const int ciphertext[], int plaintext[], int length);
int convert_cipher(int code);
int get_C(const int plaintext[], int length);
int get_K(const int plaintext[], int length);

int is_bad_code;

int main(void)
{
	int count = 0, m, i, found_double, C, K;
	int raw_data[200], data[200];
	int low = 0, high = 201, d_low = 0, d_high = 201;
	
	while (1)
	{
		// reset the situation
		found_double = 0;
		is_bad_code = 0;
		count++;
		
		scanf("%d", &m);
		if (m == 0) break;
		
		// read specified amount of data
		for (i = 0; i < m; i++)
			scanf("%d", &raw_data[i]);
		
		// initialize the border of width and double width
		low = raw_data[0];
		high = raw_data[0];
		d_low = raw_data[0];
		d_high = raw_data[0];
	
		for (i = 0; i < m; i++) {
			// find double width if not found yet
			if (!found_double) {
				if (raw_data[i] >= high * 1.9 / 1.05 && raw_data[i] <= low * 2.1 / 0.95) {
					d_low = low * 2;
					d_high = high * 2;
					found_double = 1;
				} else if (raw_data[i] >= d_high * 0.95 / 2.1 && raw_data[i] <= d_low * 1.05 / 1.9) {
					low = d_low / 2;
					high = d_high / 2;
				} else;
			}
			
			// get new borders of width and double width
			if (raw_data[i] >= low && raw_data[i] <= low * 1.05 / 0.95 && raw_data[i] >= high)
				high = raw_data[i];
			if (raw_data[i] <= high && raw_data[i] >= high * 0.95 / 1.05 && raw_data[i] <= low)
				low = raw_data[i];
			if (raw_data[i] >= d_low && raw_data[i] <= d_low * 1.05 / 0.95 && raw_data[i] >= d_high)
				d_high = raw_data[i];
			if (raw_data[i] <= d_high && raw_data[i] >= d_high * 0.95 / 1.05 && raw_data[i] <= d_low)
				d_low = raw_data[i];
		}
		
		// check if bad code exists
		if (!found_double) 
			is_bad_code = 1;
		for (i = 0; i < m; i++) {
			if (!(raw_data[i] >= low && raw_data[i] <= high) && !(raw_data[i] >= d_low && raw_data[i] <= d_high))
				is_bad_code = 1;
		}
		if ((m + 1) % 6 != 0 && m >= 29)
			is_bad_code = 1;
		
		if (is_bad_code) {
			printf("Case %d: bad code\n\n", count);
			continue;
		}
		
		/*
		printf("low:    %d\n", low);
		printf("high:   %d\n", high);
		printf("d_low:  %d\n", d_low);
		printf("d_high: %d\n", d_high);
		*/
		
		// turn raw data into 0 and 1
		int data[m];
		for (i = 0; i < m; i++) {
			if (raw_data[i] >= low && raw_data[i] <= high)
				data[i] = 0;
			else if (raw_data[i] >= d_low && raw_data[i] <= d_high)
				data[i] = 1;
		}
		
		// check if need to reverse
		if (data[0] == 0 && data[1] == 1 && data[2] == 1 && data[3] == 0 && data[4] == 0
			  && data[m - 5] == 0 && data[m - 4] == 1 && data[m - 3] == 1 && data[m - 2] == 0 && data[m - 1] == 0) {
			for (i = 0; i < m; i++) {
				if (raw_data[i] >= low && raw_data[i] <= high)
					data[m-i-1] = 0;
				else if (raw_data[i] >= d_low && raw_data[i] <= d_high)
					data[m-i-1] = 1;
			}
		}
		
		// check start and stop code
		if (data[0] != 0 || data[1] != 0 || data[2] != 1 || data[3] != 1 || data[4] != 0
			  || data[m - 5] != 0 || data[m - 4] != 0 || data[m - 3] != 1 || data[m - 2] != 1 || data[m - 1] != 0) {
			printf("Case %d: bad code\n\n", count);
			continue;
		}
		
		// check if gap bar is narrow
		for (i = 0; i < m; i++) {
			if ((i + 1) % 6 == 0 && data[i] != 0) {
				is_bad_code = 1;
			}
		}
		if (is_bad_code) {
			printf("Case %d: bad code\n\n", count);
			continue;
		}
		
		/* //display data 
		for (i = 0; i < m; i++) {
			printf("%d", data[i]);
			if ((i+1) % 6 == 0)
				printf(" ");
			if ((i+1) % 12 == 0)
				printf("\n");
		}*/
		
		int n = (m + 1) / 6 - 2;
		int plaintext[n];
		decode(data, plaintext, m);
		
		// check if bad cipher exists
		if (is_bad_code) {
			printf("Case %d: bad code\n\n", count);
			continue;
		}
		
		// check if bad C
		C = get_C(plaintext, n - 2);
		if (plaintext[n-2] != C) {
			printf("Case %d: bad C\n\n", count);
			continue;
		}
		
		// check if bad K
		K = get_K(plaintext, n - 1);
		if (plaintext[n-1] != K) {
			printf("Case %d: bad K\n\n", count);
			continue;
		}
		
		// display plaintext without C and K
		printf("Case %d: ", count);
		for (i = 0; i < n - 2; i++) {
			if (plaintext[i] == dash) {
				printf("-");
				continue;
			}
			printf("%d", plaintext[i]);
		}
		printf("\n\n");
	}
	
	return 0;
}

void decode(const int ciphertext[], int plaintext[], int length)
{
	int i, m, count, sum;
	for (i = 0, m = 6; m < length - 6; m++) {
		for (sum = 0, count = 4; count > -1; count--) {
			sum += ciphertext[m] << count;
			m++;
		}
		plaintext[i] = convert_cipher(sum);
		i++;
	}
}

int convert_cipher(int code)
{
	switch(code) {
		case 16:
			return 0;
			break;
		case 18:
			return 1;
			break;
		case 3:
			return 2;
			break;
		case 20:
			return 3;
			break;
		case 12:
			return 4;
			break;
		case 24:
			return 5;
			break;
		case 5:
			return 6;
			break;
		case 9:
			return 7;
			break;
		case 17:
			return 8;
			break;
		case 1:
			return 9;
			break;
		case 4:
			return dash;
			break;
		default:
			is_bad_code = 1;
			break;
	}
}

int get_C(const int plaintext[], int length)
{
	int sum = 0, i, n = length;
	for (i = 0; i < n; i++) {
		sum += ((n-i-1) % 10 + 1) * plaintext[i];
	}
	return sum % 11;
}

int get_K(const int plaintext[], int length)
{
	int sum = 0, i, n = length;
	for (i = 0; i < n; i++) {
		sum += ((n-i-1) % 9 + 1) * plaintext[i];
	}
	return sum % 11;
}
