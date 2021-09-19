# celestial_coordinate_transformation

赤道座標と地平座標の相互変換ツール  
精度はあまり高くありません。

## 赤道座標(RA, Dec)
- RA: 赤経  
0以上360未満までの値を指定
- Dec: 赤緯  
-90以上90以下の値を指定

## 地平座標(A, h)
- A: 方位  
0以上360未満までの値を指定
- h: 高度  
-90以上90以下の値を指定

## 使用方法
- コンパイル  
`$ g++ toolsForObs.hpp resolveCoordinates.cpp -o resolveCoordinates`

- 赤道座標から地平座標への変換  
```
$ ./resolveCoordinates -h 20 20
A=262.138, h=24.386
```

- 地平座標から赤道座標への変換  
```
$ ./resolveCoordinates -e 262.138 24.386
RA=20.322, Dec=20.000
```
