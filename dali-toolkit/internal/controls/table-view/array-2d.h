#pragma once
#ifndef __DALI_ARRAY2D_H__
#define __DALI_ARRAY2D_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

/**
 * Helper wrapper for two dimensional array using std::vector
 * Usage:
 * <code>
 *   Array2d< int > intArray( 3, 3 );
 *   intArray[ 0 ][ 0 ] = 10;
 *   intArray.Resize( 4, 4 );
 * </code>
 */
template< typename T >
class Array2d
{
public:

  /**
   * Default constructor. Creates a 0x0 array
   */
  Array2d()
  : mArray( 0, std::vector< T >( 0 ) )
  { }

  /**
   * Constructs an array with given dimensions
   * @param [in] rows for array
   * @param [in] columns for array
   */
  Array2d( unsigned int rows, unsigned int columns )
  : mArray( rows, std::vector< T >( columns ) )
  { }

  /**
   * Destructor
   */
  ~Array2d()
  { } // Nothing to do, vector cleans up itself

  /**
   * Copy constructor
   * @param array to copy from
   */
  Array2d( const Array2d& array )
  {
    if( this != &array )
    {
      mArray = array.mArray;
    }
  }

  /**
   * Assignment operator
   * @param array to copy from
   * @return reference to self for chaining
   */
  Array2d& operator=( const Array2d& array )
  {
    if( this != &array )
    {
      mArray = array.mArray;
    }
  return *this;
  }

  /**
   * @return the number of rows in the array
   */
  unsigned int GetRows()
  {
    return mArray.size();
  }

  /**
   * @return the number of columns in the array
   */
  unsigned int GetColumns()
  {
    if( mArray.size() > 0 )
    {
      // all columns are equal length
      return mArray[ 0 ].size();
    }
    return 0;
  }

  /**
   * @param [in] index of the row
   * @return reference to the row vector for given index
   */
  std::vector< T >& operator[]( unsigned int index )
  {
    return mArray[ index ];
  }

  /**
   * @param [in] index of the row
   * @return const reference to the row vector for given index
   */
  const std::vector< T >& operator[]( unsigned int index ) const
  {
    return mArray[ index ];
  }

  /**
   * Insert a new row to given index
   * @param [in] rowIndex of the new row
   */
  void InsertRow( unsigned int rowIndex )
  {
    // insert default initialized row of elements
    mArray.insert( mArray.begin() + rowIndex, std::vector< T >( GetColumns() ) );
  }

  /**
   * Delete a row from given index
   * Removed elements are deleted
   * @param [in] rowIndex of the row to delete
   */
  void DeleteRow( unsigned int rowIndex )
  {
    // erase the row
    mArray.erase( mArray.begin() + rowIndex );
  }

  /**
   * Delete a row from given index
   * @param [in] rowIndex of the row to delete
   * @param [out] removed elements
   */
  void DeleteRow( unsigned int rowIndex, std::vector< T >& removed )
  {
    // copy the row elements
    removed.insert( removed.end(), mArray[ rowIndex ].begin(), mArray[ rowIndex ].end() );
    // erase the row
    mArray.erase( mArray.begin() + rowIndex );
  }

  /**
   * Insert a new column to given index
   * @param [in] columnIndex of the new column
   */
  void InsertColumn( unsigned int columnIndex )
  {
    // go through all rows
    const unsigned int rows = GetRows();
    for( unsigned int i = 0; i < rows; ++i )
    {
      // insert default initialized element
      mArray[ i ].insert( mArray[ i ].begin() + columnIndex, T() );
    }
  }

  /**
   * Delete a column from given index.
   * Removed elements are deleted
   * @param [in] columnIndex of the column to delete
   */
  void DeleteColumn( unsigned int columnIndex )
  {
    // go through all rows
    const unsigned int rows = GetRows();
    for( unsigned int i = 0; i < rows; ++i )
    {
      // erase the column
      mArray[ i ].erase( mArray[ i ].begin() + columnIndex );
    }
  }

  /**
   * Delete a column from given index
   * @param [in] columnIndex of the column to delete
   * @param [out] removed elements
   */
  void DeleteColumn( unsigned int columnIndex, std::vector< T >& removed )
  {
    // go through all rows
    const unsigned int rows = GetRows();
    for( unsigned int i = 0; i < rows; ++i )
    {
      // copy the column element of this row
      removed.push_back( mArray[ i ][ columnIndex ] );
      // erase the column
      mArray[ i ].erase( mArray[ i ].begin() + columnIndex );
    }
  }

  /**
   * Resizes the array to given dimensions
   * If new size is smaller in either dimension, items that wont fit will be deleted
   * @param [in] rows for array
   * @param [in] columns for array
   */
  void Resize( unsigned int rows, unsigned int columns )
  {
    // resize rows first, may increase or decrease size
    mArray.resize( rows );
    for( unsigned int i = 0; i < rows; ++i )
    {
      // resize each column, may increase or decrease size
      mArray[ i ].resize( columns );
    }
  }

  /**
   * Resizes the array to given dimensions
   * If new size is smaller, items that wont fit will be returned to caller
   * @param [in] rows for array
   * @param [in] columns for array
   * @param [out] removed elements in case array is smaller in either dimension
   */
  void Resize( unsigned int rows, unsigned int columns, std::vector< T >& removed )
  {
    // remember old counts
    const unsigned int oldRows = GetRows();
    const unsigned int oldColumns = GetColumns();
    // are rows being removed ?
    if( rows < oldRows )
    {
      // gather the elements of removed rows
      for( unsigned int i = rows; i < oldRows; ++i )
      {
        // copy the row elements, the whole row is gone
        removed.insert( removed.end(), mArray[ i ].begin(), mArray[ i ].end() );
      }
    }
    // resize the rows, may increase or decrease size
    mArray.resize( rows );
    // process columns, need to do all rows as also columns for new row need resizing
    for( unsigned int i = 0; i < rows; ++i )
    {
      // if this is an old row and columns are being removed
      if( ( i < oldRows )&&( columns < oldColumns ) )
      {
        // copy the columns, the end of row from columns is gone
        removed.insert( removed.end(), mArray[ i ].begin() + columns, mArray[ i ].end() );
      }
      // resize the column, may increase of decrease size
      mArray[ i ].resize( columns );
    }
  }

private:

  std::vector< std::vector< T > > mArray;

};

} // namespace Dali

#endif // __DALI_ARRAY2D_H__
