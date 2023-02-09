# 12ステップで作る 組込みOS自作入門 devcontainer

<br>

[12ステップで作る組込みOS自作入門](https://www.amazon.co.jp/12%E3%82%B9%E3%83%86%E3%83%83%E3%83%97%E3%81%A7%E4%BD%9C%E3%82%8B%E7%B5%84%E8%BE%BC%E3%81%BFOS%E8%87%AA%E4%BD%9C%E5%85%A5%E9%96%80-%E5%9D%82%E4%BA%95-%E5%BC%98%E4%BA%AE/dp/4877832394)

<br>

## [H8/3069F](https://akizukidenshi.com/catalog/g/gK-01271/) emulator included  

[original h8_emulator](https://github.com/shimomura1004/h8_emulator)

<br><br><br><br>

## Build

<br>

#### Emulator  

F7 key

<br><br>

#### OS

<br>

```
cd src/12step_os/12
```

##### build bootload
```
make boot
```

##### build os
```
make os
```

##### bild bootload + os
```
make
```

##### clean
```
make clean
```

<br><br><br>
<br><br><br>







https://user-images.githubusercontent.com/10168979/217858856-b1b947dc-58cd-4fd4-a1c1-8a752c710201.mp4



<br><br><br>


## Usage

<br>

#### cd exe directory
```
./h8emu kzload.elf
```

#### split terminal (ctrl + shift + 5)
```
./sender ser1
```

<br>

kzload>
```
load
```
press ":" key  
(command)
```
send kozos
```
kzload>
```
run
```




<br><br><br>

quit  
press ctrl + c

<br><br><br>

F5  
debug emulator

<br><br><br>
<br><br><br>
<br><br><br>
