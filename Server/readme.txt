------Message structure:-------

[KettleID] {cups|names|name|numbers|number} {cupids}

[] = REQUIRED
{} = OPTIONAL

You can only supply 1 KettleId, 1 of the 2nd parameter and N-times of the 3rd parameter

-------Return message structure (data item separated by spaces):------
(item1), (item2), (item3), ..., (itemN)


-----Parameters------

PARAMETER 1 :: KettleID
You always have to include the kettle id when you are trying to retrieve data 
as only the kettle should be allowed to do that.

PARAMETER 2 :: cups | names | name | numbers | number 
This will return the following:
cups 	: all the cup ids
names 	: all the names
numbers	: all the numbers
name	: names for the provided cup ids
number	: numbers for the provided cup ids

PARAMETER 3 :: {cupids}
For a parameter 2 where you require to give input, this is where you give it.
You can give as many as you please.

-----Examples-----

(Omit the starting "--" at the beginning of each example)

Get all the cup ids:
-- kettle1
OR
-- kettle1 cups

Get all the numbers
-- kettle1 numbers

Get specific name
-- kettle1 name m007
