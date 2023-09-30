#ifndef ZTABLE_H
#define ZTABLE_H

#include "zarray.h"
#include "zmap.h"

namespace LibChaos {

//! Two-dimensional table container with column names.
class ZTable {
public:
    ZTable();
    ZTable(ZArray<ZString> columns);

    //! Add a column to the table.
    void addColumn(ZString name);
    //! Adda record to the table.
    void addRecord(ZArray<ZString> record);
    
    //! Clear columns and records.
    void clear();

    //! Get reference to field in \a column and \a row.
    ZString &field(zu64 column, zu64 row);
    //! Get reference to field in \a column and \a row.
    ZString &field(ZString column, zu64 row);
    //! Get reference to record in \a row.
    ZArray<ZString> &record(zu64 row);

    //! Get an array of column names.
    ZArray<ZString> getColumnNames();
    //! Get an array of fields in \a column.
    ZArray<ZString> getColumn(ZString name);
    //! Get a map of column names to fields in \a row.
    ZMap<ZString, ZString> getRecord(zu64 row);

    //! Number of columns.
    zu64 colCount();
    //! Number of rows.
    zu64 rowCount();

private:
    ZMap<ZString, zu64> _columns;
    ZArray<ZArray<ZString>> _records;
};

}

#endif // ZTABLE_H
