#include "lv_persian.h"

persian_map_t*lv_find_persian_letter(uint32_t letterCode) {
	for (int i = 0; i < sizeof(persian_lookup_table); i++) {
		if (persian_lookup_table[i].letter_code == letterCode) {
			return (persian_map_t*) &persian_lookup_table[i];
		}
	}
	return NULL;
}

uint32_t lv_get_converted_persian_letter(uint32_t preLetter, uint32_t letter,
		uint32_t nextLetter) {
	persian_map_t *nextPersianLetter = NULL, *persianLetter = NULL,
			*prePersianLetter = NULL;
	if ((preLetter > 0x600) && (preLetter < 0x6FF)) {
		prePersianLetter = lv_find_persian_letter(preLetter);
	}
	persianLetter = lv_find_persian_letter(letter);
	if ((nextLetter > 0x600) && (nextLetter < 0x6FF)) {
		nextPersianLetter = lv_find_persian_letter(nextLetter);
	}
	if (persianLetter != NULL) {
		if (prePersianLetter != NULL) {
			if (nextPersianLetter != NULL) {
				if (nextPersianLetter->connect_to_next) {
					return persianLetter->middle_code;
				} else {
					if (prePersianLetter->connect_to_previous) {
						return persianLetter->prefix_code;
					} else {
						return persianLetter->alone_code;
					}
				}
			} else {
				if (prePersianLetter->connect_to_previous) {
					return persianLetter->prefix_code;
				} else {
					return persianLetter->alone_code;
				}
			}

		} else {
			if (nextPersianLetter != NULL) {
				if (nextPersianLetter->connect_to_next) {
					return persianLetter->suffix_code;
				} else {
					return persianLetter->alone_code;
				}
			} else {
				return persianLetter->alone_code;
			}
		}
	}
	return letter;
}

uint8_t* lv_get_reversed_buffer(uint8_t*buffer, uint16_t start, uint16_t end) {
	struct text_block_s block;
	struct text_block_s *current_block;
	struct text_block_s *last_block;
	bool contain_persian_letter = false;
	if ((end - start) < 3) {
		return buffer;
	}
	for (int i = start; i < end; i++) {
		if ((*((uint16_t*) &buffer[i]) & 0xFC) == 0xD8) {
			contain_persian_letter = true;
			break;
		}
	}
	if (!contain_persian_letter) {
		return buffer;
	}
	uint8_t* reversed_buff = lv_mem_alloc(end - start + 2);
	if(reversed_buff == NULL){
		return buffer;
	}
	*((uint32_t*)&reversed_buff[end-start-2]) = NULL;
	current_block = &block;
	block.next = NULL;
	block.start = start;
	block.end = end;
	for (int i = end - 1; i > start; i--) {
		if ((buffer[i - 1] & 0xFC) == 0xD8) {
			current_block->next = lv_mem_alloc(sizeof(struct text_block_s));
			current_block = current_block->next;
			current_block->next = NULL;
			current_block->end = i - 1;
			current_block->start = i - 1;
			current_block->utf = true;
			while (((*((uint16_t*) &buffer[current_block->start - 2]) & 0xFC)
					== 0xD8) && (i > start)) {
				current_block->start -= 2;
				i -= 2;
			}
		} else {
			current_block->next = lv_mem_alloc(sizeof(struct text_block_s));
			current_block = current_block->next;
			current_block->next = NULL;
			if (i == end - 1) {
				current_block->end = end;
			} else {
				current_block->end = i;
			}
			current_block->start = i;
			current_block->utf = false;
			while ((*((uint16_t*) &buffer[current_block->start - 2]) & 0xFC)
					!= 0xD8 && (i > start)) {
				current_block->start--;
				i--;
			}
		}
	}
	current_block = &block;
	uint16_t length = 0;
	do {
		current_block = current_block->next;
		if (current_block->utf) {
			do {
				*((uint16_t*) &reversed_buff[length]) =
						*((uint16_t*) &buffer[current_block->end]);
				current_block->end -= 2;
				length += 2;
			} while (current_block->end >= current_block->start);
		} else {
			do {
				reversed_buff[length] = buffer[current_block->start];
				current_block->start++;
				length++;
			} while (current_block->start < current_block->end);
		}
	} while (current_block->next != NULL);
	current_block = &block;
	last_block = current_block->next;
	while (block.next != NULL) {
		while (last_block->next != NULL) {
			current_block = last_block;
			last_block = last_block->next;
		}
		lv_mem_free(last_block);
		current_block->next = NULL;
		current_block = &block;
		last_block = block.next;
	}
	return reversed_buff;
}
