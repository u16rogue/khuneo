# khuneo
![45268474_p02_64](https://user-images.githubusercontent.com/61908580/232050252-2580a7d2-8ae2-4466-8024-011ef4cb4585.png)<br>
khuneo programming language - A modular and abstracted embedded programming language<br>
[GitLab](https://gitlab.com/u16rogue/khuneo)<br>
<hr>

(Planned) Syntax
```
import std;
import c_ffi as ffi;

def somestruct = ffi#def_struct {
  x: i32;
  y: f32;
};

somestruct.$add = (self, other) {
  return {
    x = self.x + other.x,
    y = self.y + other.y,
    $type = somestruct,
  };
};

def arrays = [1, 2, 3, 4];
def dictionaries = { one = 1, two: i32 = 2 };
def mutable_var: i32! = 1234;
mutable_var = 5678;

compiler#set("entrypoint")
def main(args) i32 {
  def n: i32 = args.count as i32;
  if (features#has("raw_memory") && !features#is("sanboxed")) {
    def ptr = 0xDEADBEEF;
    std.print("x is \{(ptr as somestruct).x)\}";
    ptr.$type = somestruct;
    std.print("y is \{ptr.y\}");
  };

  iter as args_loop (arg : args) {
    if (arg != "-a")
      break;
    iter (x : n-1) {
      if (args[x] == arg)
        break args_loop;
    };

    iter {
      def mem = ffi.malloc(4);
      defer { ffi.free(mem); };
    } (false);
  };

  return n;
}
```
