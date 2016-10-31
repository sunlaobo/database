#pragma once

//	Database transactions: housed in database ObjId slots

typedef struct {
	uint64_t timestamp;	// txn timestamp, reader or writer
	DbAddr frame[1];	// contains DocIds in the TXN
	ObjId txnId;		// where we are stored.
} Txn;

//  txn command enum:

typedef enum {
	TxnAddDoc,
	TxnDelDoc,
	TxnUpdDoc
} TxnCmd;
	
void addDocToTxn(DbMap *database, Txn *txn, ObjId docId, TxnCmd cmd);
Doc *findDocVer(DbMap *docStore, ObjId docId, Txn *txn);
