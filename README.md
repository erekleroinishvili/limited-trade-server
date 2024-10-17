# Limited Trade Server

## Description

This is a C++ app which reads trade orders and executes them against the order book, outputting the resulting trade messages and the state of the order book.

## Compiling

This application should be compiled by a C++ compiler to produce a command-line executable.

The entry file is `setsmm-simulator.cpp` which includes all dependencies. So, the C++ compiler should receive this file name as input.

The following sample command could be used, assuming the presence of `G++` compiler:

On Windows OS:

* `g++ -std=gnu++17 setsmm-simulator.cpp -o f`

On Linux-based OS:

* `g++ -std=gnu++17 setsmm-simulator.cpp -o setsmm-simulator`

After compiling the executable will be placed in file `setsmm-simulator.exe` on windows, or in `setsmm-simulator` on a Linux-based OS. This is a command-line executable. When run, it will expect input from standard input and respond on standard output.

## Input

This simulator accepts two types of order placement commands: place limit order and place iceberg order. Other order types are not supported. Orders cannot be modified or cancelled.

Input is received from stdin in the form of individual lines delimited by newlines. A line may consist entirely of whitespace characters (empty); begin with zero or more whitespace characters followed by the # character and subsequently any other characters (comment); in comma-separated ascii format (data). Lines formatted as empty or comment shall be ignored. You may assume that lines formatted as data will always be correctly formatted, with no deviation from the specified types and no overflow (for simplicity, all prices are in whole pence, no fractions are allowed). Data lines may be one of [Insert Limit Order](#insert-limit-order) or [Insert Iceberg Order](#insert-iceberg-order).

### Insert Limit Order

This message contained within a single line, with the following comma separated values:

1. Character `B` for a buy order, `S` for a sell order
1. Unique order identifier (positive integer)
1. Price in pence (positive integer)
1. Quantity of order (positive integer)

Example:\
B,100322,5103,7500

Example Description:\
Limit order id 100322: Buy 7,500 at 5,103p,

### Insert Iceberg Order

This message is practically identical to an insert limit order message, except that it contains 1 extra field specifying the peak size. It is again contained within a single line, with comma separated values. Unlike the associated [technical document](./icebergs-technical-description.pdf), for the purposes of this simulation iceberg orders only have a single order identifier (no hidden identifier) that will be used in trade messages.

1. Character `B` for a buy order, `S` for a sell order
1. Unique order identifier (positive integer)
1. Price in pence (positive integer)
1. Quantity of order (positive integer)
1. Peak size (positive integer)

Example:\
S,100345,5103,100000,10000

Example Description:\
Iceberg order id 100345: Sell 100,000 at 5103p, with a peak size of 10,000

## Output

All non-error output is directed to stdout. For each order insert message the solution applies the order to the current book, and generates any matched trades first in the order of matching precedence (i.e. price and time precedence of the orders), and then output the current book.

### Trade Format

A trade message is sent to stdout for each matched trade. As stated by the [technical specification](./icebergs-technical-description.pdf), only a single trade message is generated for each iceberg order, even if the match occurs on more than one peak (e.g. a match size of 16,000 on an iceberg order with a peak size of 10,000 should trade once for a quantity of 16,000, not one for 10,000 and another for 6,000).

1. Buy order id matched
1. Sell order id matched
1. Price in pence
1. Quantity

Example:\
100322,100345,5103,7500

### Book format

Following the receipt of an order message, and after resolving any matches in the book and outputting any generated trade messages, the app displays the current full order book in the following example format:

* Id columns width (excluding formatting marks) = 10
* Volume columns width (excluding formatting marks) = 13
* Price columns width (excluding formatting marks) = 7
* Total width including 7 formatting marks = 67

All headers left justified with single space between formatting mark and start of text
All data fields right-justified with no space between data and formatting mark
Order Ids should be formatted as a plain number (i.e. 103749 not 103,749), but
volumes and prices should be formatted with commas (i.e. 100,000 not 100000)

```text
+-----------------------------------------------------------------+
| BUY                            | SELL                           |
| Id       | Volume      | Price | Price | Volume      | Id       |
+----------+-------------+-------+-------+-------------+----------+
|1234567890|1,234,567,890| 32,503| 32,504|1,234,567,890|1234567891|
|      1138|        7,500| 31,502| 32,505|        7,777|      6808|
|          |             |       | 32,507|        3,000|     42100|
+-----------------------------------------------------------------+
```

**Note 1**: the volume displayed for iceberg orders is the current peak volume,
not total volume.

**Note 2**: the order book entries are displayed in priority order.

## Testing

On a Windows machine, simply run

* `run-tests.bat`

The command will compile the source code and execute it against tests in the `Tests` folder.
