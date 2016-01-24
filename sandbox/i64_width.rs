fn decimal_len(decimal: i64) -> usize {
    use std::i64;
    
    match decimal {
        i64::MIN...-1000000000000000000 => 19,
        -999999999999999999...-100000000000000000 => 18,
        -99999999999999999...-10000000000000000 => 17,
        -9999999999999999...-1000000000000000 => 16,
        -999999999999999...-100000000000000 => 15,
        -99999999999999...-10000000000000 => 14,
        -9999999999999...-1000000000000 => 13,
        -999999999999...-100000000000 => 12,
        -99999999999...-10000000000 => 11,
        -9999999999...-1000000000 => 10,
        -999999999...-100000000 => 9,
        -99999999...-10000000 => 8,
        -9999999...-1000000 => 7,
        -999999...-100000 => 6,
        -99999...-10000 => 5,
        -9999...-1000 => 4,
        -999...-100 => 3,
        -99...-10 => 2,
        -9...9 => 1,
        10...99 => 2,
        100...999 => 3,
        1000...9999 => 4,
        10000...99999 => 5,
        100000...999999 => 6,
        1000000...9999999 => 7,
        10000000...99999999 => 8,
        100000000...999999999 => 9,
        1000000000...9999999999 => 10,
        10000000000...99999999999 => 11,
        100000000000...999999999999 => 12,
        1000000000000...9999999999999 => 13,
        10000000000000...99999999999999 => 14,
        100000000000000...999999999999999 => 15,
        1000000000000000...9999999999999999 => 16,
        10000000000000000...99999999999999999 => 17,
        100000000000000000...999999999999999999 => 18,
        1000000000000000000...i64::MAX => 19,
        _ => unreachable!()
    }
}
