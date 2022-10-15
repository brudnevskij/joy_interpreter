permBase xs = xs:((tail xs)++(head xs):[] ):[]

count:: [a]->Int
count xs
       | null xs = 0
       | otherwise = 1 + count (tail xs)

appendToAll :: a -> [[a]] -> [[a]]
appendToAll x xs
               | null xs = []
               | otherwise = (x:(head xs)):appendToAll x (tail xs)

getByN n (x:xs)
              | n == 0 = x
              | otherwise = getByN (n-1) xs

delByN :: Int -> [a] -> [a]
delByN n [] = []
delByN n (x:xs)
              |n == 0 = xs
              |otherwise = x:delByN (n-1) xs


permut n xs
        | count xs == 2 = permBase xs
        | n == 0 = []
        | otherwise = appendToAll (getByN (n-1) xs) (permut (count (tail xs)) (delByN (n-1) xs)) ++ permut (n-1) xs

genPerms xs = permut (length xs) xs



main:: IO()
main =
      do
       let a = genPerms [1..8]
       print(length a)
--       let b = genPerms [1..8]
--       print(length b)
--       let c = genPerms [1..9]
--       print(length c)
--       print(genPerms [1..6])
--       print(genPerms [1..5])
--       print(genPerms [1..8])
--       print(genPerms [1, 3, 2, 1, 5])




