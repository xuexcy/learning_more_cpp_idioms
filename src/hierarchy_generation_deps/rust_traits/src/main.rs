trait Dance {
    fn dance(&self);
}
trait Joke {
    fn joke(&self);
}
trait Song {
    fn sing(&self);
}

trait DanceA : Dance {
  fn dance(&self) { println!("dance A"); }
}

trait DanceB : Dance {
  fn dance(&self) { println!("dance B"); }
}

trait DanceC : Dance {
  fn dance(&self) { println!("dance C"); }
}

trait JokeA : Joke {
  fn joke(&self) { println!("joke A"); }
}

trait JokeB : Joke {
  fn joke(&self) { println!("joke B"); }
}

trait JokeC : Joke {
  fn joke(&self) { println!("joke C"); }
}

trait SongA : Song {
  fn sing(&self) { println!("sing A"); }
}

trait SongB : Song {
  fn sing(&self) { println!("sing B"); }
}

trait SongC : Song {
  fn sing(&self) { println!("sing C"); }
}
struct X;
struct Y;
struct Z;
impl DanceA for X {}
impl JokeB for X {}
impl SongC for X {}
impl Dance for X {
    fn dance(&self) {
        <Self as DanceA>::dance(self);
    }
}
impl Joke for X {
    fn joke(&self) {
        <Self as JokeB>::joke(self);
    }
}
impl Song for X {
    fn sing(&self) {
        <Self as SongC>::sing(self);
    }
}

impl DanceB for Y {}
impl JokeA for Y {}
impl SongA for Y {}
impl Dance for Y {
    fn dance(&self) {
        <Self as DanceB>::dance(self);
    }
}
impl Joke for Y {
    fn joke(&self) {
        <Self as JokeA>::joke(self);
    }
}
impl Song for Y {
    fn sing(&self) {
        <Self as SongA>::sing(self);
    }
}

fn entertain<T: Dance + Joke + Song>(t: &T) {
    t.sing();
    t.dance();
    t.joke();
}

fn main() {
    let x = X;
    entertain(&x);

    let y = Y;
    entertain(&y);
}
