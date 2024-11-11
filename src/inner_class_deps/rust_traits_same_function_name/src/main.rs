trait Base1 {
    fn open(&self, i: i32) -> i32;
}
trait Base2 {
    fn open(&self, i: i32) -> i32;
}
struct Derived {}
impl Base1 for Derived {
    fn open(&self, i: i32) -> i32 {
        let res = i * 2;
        println!("i: {}, i * 2: {}", i, res);
        return res;
    }
}
impl Base2 for Derived {
    fn open(&self, i: i32) -> i32 {
        let res = i * i;
        println!("i: {}, i * i: {}", i, res);
        return res;
    }
}

fn run() {
    let d = Derived{};
    println!("{}", Base1::open(&d, 3));
    println!("{}", Base2::open(&d, 3));
}
fn main() {
    run();
}
