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
-- kettle2
OR
-- kettle2 cups

Get all the numbers
-- kettle2 numbers

Get specific name
-- kettle2 name d163bbdd530ec035

------Ids---------

Kettle Ids: 485322d4e0f44c3d kettle2 kettle3 kettle4 kettle5
Cup Ids: d163bbdd530ec035 36a5c5d5cc28739a e164bcde513cf831 aj385763j1rl389l 3jd2j685kld930s1
Names: alek catalina ewan georgi markus

Only the 1st kettle id and the first 2 cup ids are actual devices.
