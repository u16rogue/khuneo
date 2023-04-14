# khuneo
![45268474_p02_64](https://user-images.githubusercontent.com/61908580/232050252-2580a7d2-8ae2-4466-8024-011ef4cb4585.png)<br>
khuneo programming language - A modular and abstracted embedded programming language<br>
[GitLab](https://gitlab.com/u16rogue/khuneo)<br>
<hr>

(Planned) Syntax
```
import std;
import ffi;

const somestruct = ffi#def_struct {
  x: i32;
  y: f32;
};

somestruct.$add = fn (self, other) {
  return {
    x = self.x + other.x,
    y = self.y + other.y,
    $type = somestruct,
  };
};

let arrays = [1, 2, 3, 4];
let dictionaries = { one = 1, two: i32 = 2 };

compiler#set("entrypoint")
fn main(args) i32 {
  let n: i32 = args.count as i32;
  if (features#has("raw_memory") && !features#is("sanboxed")) {
    let ptr = 0xDEADBEEF;
    std.print("x is \{(ptr as somestruct).x)\}";
    ptr.$type = somestruct;
    std.print("y is \{ptr.y\}");
  }
  return n;
}
```
