#include "../db.h"
#include "../db_object.h"
#include "../db_arena.h"
#include "../db_index.h"
#include "../db_map.h"
#include "btree1.h"

Status btree1FindKey( DbCursor *dbCursor, DbMap *map, uint8_t *key, uint32_t keyLen, bool onlyOne) {
Btree1Cursor *cursor = (Btree1Cursor *)dbCursor;
Btree1Index *btree1 = btree1index(map);
uint8_t *foundKey;
Btree1Set set[1];
Status stat;

	// find the level 0 page containing the key

	if ((stat = btree1LoadPage(map, set, key, keyLen, 0, Btree1_lockRead, false)))
		return stat;

	foundKey = keyptr(set->page, set->slotIdx);
	cursor->base->state = CursorPosAt;

	if (onlyOne) {
		int rawLen = keylen(foundKey) + keypre(foundKey);
		memset (cursor->page, 0, sizeof(Btree1Page));
		cursor->page->cnt = 2;
		cursor->page->act = 2;

		cursor->page->min = btree1->pageSize;
		cursor->page->min <<= btree1->leafXtra;
		cursor->page->min -= 1;
		((uint8_t *)cursor->page)[cursor->page->min] = 0;

		slotptr(cursor->page, 2)->bits = cursor->page->min;
		slotptr(cursor->page, 2)->type = Btree1_stopper;

		cursor->page->min -= rawLen;
		slotptr(cursor->page, 1)->bits = cursor->page->min;

		memcpy (keyptr(cursor->page,1), foundKey + keypre(foundKey), keylen(foundKey));

		cursor->base->key = keyptr(cursor->page, 1);
		cursor->base->keyLen = keylen(foundKey);
		cursor->slotIdx = 1;
		return OK;
	}

	memcpy(cursor->page, set->page, btree1->pageSize);
	btree1UnlockPage(set->page, Btree1_lockRead);

	cursor->base->key = foundKey + keypre(foundKey);
	cursor->base->keyLen = keylen(foundKey);
	cursor->slotIdx = set->slotIdx;
	return OK;
}