# -*- coding: utf-8 -*-

import os
import itertools
import struct
import hashlib
import hashlib as slowsha
import sys

import logic

# Максимальная разница количества бит 1 и 0 в процентах
ONES_AND_ZEROS_MAX_DIFF = 12
# Минимальное количество источников случайных событий, инициализирующих
# генератор (клавиатура, время, мышь и прочее)
MIN_RANDOM_SOURCES_NUMBER = 2

strFromHex = lambda s: ''.join(chr(int(s[i:i + 2], 16)) for i in range(0, len(s), 2))
hash256 = lambda s: hashlib.sha256(bytearray(s)).hexdigest()
hash256int = lambda s: int(slowsha.sha256(bytearray(s)).hexdigest(), 16)
ones_count = lambda seed: bin(seed).count('1')
zeros_count256 = lambda seed: 256 - bin(seed).count('1')


class GeneratorError(Exception):
    pass


def validate_initialisation_vector(vector):
    '''vector - набор источников случайных событий для начального заполнения
    регистров генератора (список строк)'''
    if len(vector) < MIN_RANDOM_SOURCES_NUMBER:
        raise GeneratorError(
                'minimum number of sources is %d, but %d given' % (MIN_RANDOM_SOURCES_NUMBER, len(vector))
            )
    # получаем хэш из источника (например, системного таймера и
    # клавиатуры), это и есть зерно
    # получаем количество нулей и единиц в битах хэша
    seed_int = hash256int(''.join(vector))
    ones = ones_count(seed_int)
    zeros = zeros_count256(seed_int)
    # проверяем баланс единиц-нулей
    # (относительную разницу между нулями и единицами)
    diff = max(
        abs(ones - zeros) / float(ones),
        abs(ones - zeros) / float(zeros)
    ) * 100
    if diff > ONES_AND_ZEROS_MAX_DIFF:
        raise GeneratorError(u'Seed ones/zeros balance test failed')


def bytes_to_int_be(s):
    '''char (big-endian) -> int'''
    # определяем количество байт, в которое умещается x
    x_bytes = len(s)
    # создаем формат, состоящий из x_bytes символов 'B'
    struct_format = '<' + ''.join('B' for x in xrange(x_bytes))
    # получаем список байт числа x в порядке little-endian
    parts = reversed(struct.unpack(struct_format, s))
    # сворачиваем все байты числа x в int
    x = 0
    i = 0
    for part in parts:
        x |= (part << (i * 8))
        i += 1
    return x


def make_key_from_int(key_int):
    return (
        (key_int >> (256 - 32 * 1)) & 0xffffffff,
        (key_int >> (256 - 32 * 2)) & 0xffffffff,
        (key_int >> (256 - 32 * 3)) & 0xffffffff,
        (key_int >> (256 - 32 * 4)) & 0xffffffff,
        (key_int >> (256 - 32 * 5)) & 0xffffffff,
        (key_int >> (256 - 32 * 6)) & 0xffffffff,
        (key_int >> (256 - 32 * 7)) & 0xffffffff,
        (key_int >> (256 - 32 * 8)) & 0xffffffff,
    )


def make_gl():
    gl = logic.GenerationLogic()

    a = []
    while True:
        try:
            a.append(os.urandom(512))
            validate_initialisation_vector(a)
            break
        except GeneratorError:
            pass

    seed_chars = strFromHex(hash256(''.join(a)))
    key_cfb = make_key_from_int(bytes_to_int_be(seed_chars))

    seed_chars = strFromHex(hash256(seed_chars))[:8]
    vector_cfb = bytes_to_int_be(seed_chars)

    gl.seed(vector_cfb, key_cfb, (struct.unpack('<Q', os.urandom(8))[0] for i in itertools.repeat(None)))

    return gl


def main():
    gl = make_gl()
    
    #f = open('random.dat', 'wb')
    #for i in xrange(156250000):
    #    f.write(struct.pack('>Q', gl.random64()))
    #f.flush()
    #f.close()
    
    gl = make_gl()
    
    f = open('min.dat', 'wb')
    for i in xrange(15625):
        f.write(struct.pack('>Q', gl.random64()))
    f.flush()
    f.close()
    return 0


if __name__ == '__main__':
    main()
