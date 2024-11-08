use std::fmt::{Display, LowerHex};

const fn is_big_endian() -> bool {
    let n: u32 = 0x1_2_3_4;
    let bytes = n.to_ne_bytes(); // 转换为字节数组
    bytes[0] == 0x1 // 如果第一个字节是 0x1，说明是大端
}

trait SelfSize: Sized {
    const SIZE: usize = std::mem::size_of::<Self>(); // = std::mem::size_of::<T>();
}
impl SelfSize for Point {}
const _: () = assert!(8 == Point::SIZE);
impl SelfSize for Circle {}
const _: () = assert!(16 == Circle::SIZE);
impl SelfSize for u8 {}
const _: () = assert!(1 == u8::SIZE);
impl SelfSize for i16 {}
const _: () = assert!(2 == i16::SIZE);
impl SelfSize for i32 {}
const _: () = assert!(4 == i32::SIZE);
impl SelfSize for i64 {}
const _: () = assert!(8 == i64::SIZE);

trait HexString : LowerHex + Display + SelfSize {
    fn hex_string(&self) -> String {
        return std::format!(
            "[{:#0width$x} == {0}]",
            self,
            width = Self::SIZE * 2 + 2
        );
    }
}
impl HexString for u8 {}
impl HexString for i16 {}
impl HexString for i32 {}
impl HexString for i64 {}

trait ToString {
    fn string(&self) -> String;
}

trait Serializable : SelfSize {
    // buffer: 首先 buffer 需要向前移动，所以传参的方式是 &mut ，其次 buffer 指向一段可变 array 的首地址，
    // 即 buffer 的值类型是 *mut u8
    fn serialize(&self, buffer: &mut *mut u8, buf_size: &mut usize) -> Result<usize, &str> {
        let size_of_t: usize = Self::SIZE;
        if size_of_t > *buf_size {
            return Err("insufficient memory!");
        }
        let ptr = self as *const Self as *const u8; // 转换为字节指针
        unsafe {
            std::ptr::copy_nonoverlapping(ptr, *buffer, size_of_t);
            *buffer = (*buffer).add(size_of_t);
        }
        *buf_size -= size_of_t;
        return Ok(size_of_t);
    }
}
impl Serializable for Point {}
impl Serializable for Circle {}
impl Serializable for i32 {}


#[derive(Debug)]
struct Point {
    x: i32,
    y: i16,
}

impl ToString for Point {
    fn string(&self) -> String {
        return format!(
            "[x: i32 = {}, y: i16 = {}]",
            self.x.hex_string(),
            self.y.hex_string()
        );
    }
}

#[derive(Debug)]
struct Circle {
    center: Point,
    radius: i64,
}

impl ToString for Circle {
    fn string(&self) -> String {
        return format!(
            "[center: Point = {}, radius: i64 = {}]",
            self.center.string(),
            self.radius.hex_string()
        );
    }
}

fn run() {
    println!("Point size: {}", Point::SIZE);
    println!("Circle size: {}", Circle::SIZE);
    println!("i32 size: {}", i32::SIZE);
    const IS_BIG: bool = is_big_endian();
    println!("machine endian: {}", if IS_BIG { "big" } else { "little" });

    let mut data = [0u8; Point::SIZE + Circle::SIZE + i32::SIZE];
    // let mut buffer = &data as *const _;
    let mut buffer = &mut data as *mut _;
    let mut buf_size = data.len() * std::mem::size_of_val(&data[0]);
    assert!(28 == buf_size);
    let mut idx: usize = 0;

    {
        let old_buffer = buffer;
        let old_buf_size = buf_size;
        let p = Point {
            x: 0x12345678,
            y: 0x0ABC,
        };
        println!("Point p = {}", p.string());

        let used_size = p.serialize(&mut buffer, &mut buf_size).unwrap();

        assert!(used_size == Point::SIZE);
        assert!(unsafe { old_buffer.add(Point::SIZE) } == buffer);
        assert!(old_buf_size == Point::SIZE + buf_size);
        // p.x = 0x12345678
        // p.y = 0x0ABC
        let mut i32_x = vec![0x78_u8, 0x56, 0x34, 0x12];
        let i16_y = vec![0xBC_u8, 0x0A];
        let aligning_for_y = vec![0_u8, 0];
        let mut aligned_y = i16_y;
        aligned_y.extend_from_slice(&aligning_for_y);

        if IS_BIG {
            i32_x.reverse();
            aligned_y.reverse();
        }
        for i in 0..i32_x.len() {
            assert!(
                i32_x[i] == data[idx],
                "i32_x[{}]: {} != data[{}]: {}",
                i,
                i32_x[i].hex_string(),
                idx,
                data[idx].hex_string()
            );
            println!(
                "i32_x[{}] == {} == data[{}]",
                i,
                data[idx].hex_string(),
                idx
            );
            idx += 1;
        }
        for i in 0..aligned_y.len() {
            assert!(
                aligned_y[i] == data[idx],
                "aligned_y[{}]: {} != data[{}]: {}",
                i,
                aligned_y[i].hex_string(),
                idx,
                data[idx].hex_string()
            );
            println!(
                "aligned_y[{}] == {} == data[{}]",
                i,
                data[idx].hex_string(),
                idx
            );
            idx += 1;
        }
    }
    {
        let old_buffer = buffer;
        let old_buf_size = buf_size;
        let p = Point {
            x: 0x2468ACDF,
            y: 0x3928,
        };
        let c = Circle {
            center: p,
            radius: 0x2825628257282931,
        };
        println!("Circle c = {}", c.string());

        let used_size = c.serialize(&mut buffer, &mut buf_size).unwrap();

        assert!(used_size == Circle::SIZE);
        assert!(unsafe { old_buffer.add(Circle::SIZE) } == buffer);
        assert!(old_buf_size == Circle::SIZE + buf_size);
        // c.center.x = 0x2468ACDF
        // c.center.y = 0x3928
        // c.radius = 0x2825628257282931
        let mut i32_x = vec![0xDF_u8, 0xAC, 0x68, 0x24];
        let i16_y = vec![0x28_u8, 0x39];
        let aligning_for_y = vec![0_u8, 0];
        let mut aligned_y = i16_y;
        aligned_y.extend_from_slice(&aligning_for_y);
        let mut i64_radius = vec![0x31_u8, 0x29, 0x28, 0x57, 0x82, 0x62, 0x25, 0x28];

        if IS_BIG {
            i32_x.reverse();
            aligned_y.reverse();
            i64_radius.reverse();
        }
        for i in 0..i32_x.len() {
            assert!(
                i32_x[i] == data[idx],
                "i32_x[{}]: {} != data[{}]: {}",
                i,
                i32_x[i].hex_string(),
                idx,
                data[idx].hex_string()
            );
            println!(
                "i32_x[{}] == {} == data[{}]",
                i,
                data[idx].hex_string(),
                idx
            );
            idx += 1;
        }
        for i in 0..aligned_y.len() {
            assert!(
                aligned_y[i] == data[idx],
                "aligned_y[{}]: {} != data[{}]: {}",
                i,
                aligned_y[i].hex_string(),
                idx,
                data[idx].hex_string()
            );
            println!(
                "aligned_y[{}] == {} == data[{}]",
                i,
                data[idx].hex_string(),
                idx
            );
            idx += 1;
        }
        for i in 0..i64_radius.len() {
            assert!(
                i64_radius[i] == data[idx],
                "i64_radius[{}]: {} != data[{}]: {}",
                i,
                i64_radius[i].hex_string(),
                idx,
                data[idx].hex_string()
            );
            println!(
                "i64_radius[{}] == {} == data[{}]",
                i,
                data[idx].hex_string(),
                idx
            );
            idx += 1;
        }
    }
    {
        let old_buffer = buffer;
        let old_buf_size = buf_size;
        let num: i32 = 0x13149527;
        println!("i32 num = {}", num.hex_string());

        let used_size = num.serialize(&mut buffer, &mut buf_size).unwrap();

        assert!(used_size == i32::SIZE);
        assert!(unsafe { (old_buffer).add(i32::SIZE) } == buffer);
        assert!(old_buf_size == i32::SIZE + buf_size);

        let mut i32_num = vec![0x27u8, 0x95, 0x14, 0x13];
        if IS_BIG {
            i32_num.reverse();
        }

        for i in 0..i32_num.len() {
            assert!(
                i32_num[i] == data[idx],
                "i32_num[{}]: {} != data[{}]: {}",
                i,
                i32_num[i].hex_string(),
                idx,
                data[idx].hex_string()
            );
            println!(
                "i32_num[{}] == {} == data[{}]",
                i,
                data[idx].hex_string(),
                idx
            );
            idx += 1;
        }
    }
    {
        assert!(0 == buf_size);
        let old_buffer = &mut data as *mut u8;
        assert!(unsafe { old_buffer.add(Point::SIZE).add(Circle::SIZE).add(i32::SIZE) } == buffer);
    }
}

fn main() {
    run();
}
