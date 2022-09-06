# -*- coding: utf-8 -*-

# Константы ГОСТ 28147-89
GOST_C1 = 0x1010101
GOST_C2 = 0x1010104
# блок замены из примеров ГОСТ Р 34.11-94,
# используется в RFC 4357 (11.2) и в ЦБ РФ
CBRF_SBOX = (
    (4, 10, 9, 2, 13, 8, 0, 14, 6, 11, 1, 12, 7, 15, 5, 3),
    (14, 11, 4, 12, 6, 13, 15, 10, 2, 3, 8, 1, 0, 7, 5, 9),
    (5, 8, 1, 13, 10, 3, 4, 2, 14, 15, 12, 7, 6, 0, 9, 11),
    (7, 13, 10, 1, 0, 8, 9, 15, 14, 4, 6, 12, 11, 2, 5, 3),
    (6, 12, 7, 1, 5, 15, 13, 8, 4, 10, 9, 14, 0, 3, 11, 2),
    (4, 11, 10, 0, 7, 2, 1, 13, 3, 6, 8, 5, 9, 12, 15, 14),
    (13, 11, 4, 1, 3, 15, 5, 9, 0, 10, 14, 7, 6, 8, 2, 12),
    (1, 15, 13, 0, 5, 7, 10, 4, 9, 2, 3, 14, 6, 11, 8, 12),
)
BLOCK_SIZE = 2 ** 64
HALF_BLOCK_SIZE = 2 ** 32

# блок подстановки (преобразует блоки в 32-битном числе x с помощью
# оптимизированных блоков замены opt)
subst = lambda x, opt: opt[3][x >> 24 & 255] | opt[2][x >> 16 & 255] | opt[1][x >> 8 & 255] | opt[0][x & 255]
# циклический сдвиг влево на 11 разрядов 32-битного числа
rotl32 = lambda data32: ((data32 << 11) & 0xffffffff) | (data32 >> 21)


def sbox_optimized(sbox):
    '''Преобразует набор блоков замены в оптимизированные блоки замены
    аналогично реализации шифрования ООО "Криптоком"'''
    optimized = [[], [], [], []]
    for i in xrange(256):
        for j in xrange(4):
            optimized[j].append((sbox[2 * j + 1][i >> 4] << 4 | sbox[2 * j][i & 15]) << 8 * j)
    for i in xrange(4):
        optimized[i] = tuple(optimized[i])
    return tuple(optimized)

CBRF_SBOX_OPT = sbox_optimized(CBRF_SBOX)


def encrypt_ecb(data64, key):
    '''Шифрование в режиме простой замены (The Electronic Codebook Mode
    encryption) по ГОСТ 28147-89.
    data64 - блок для шифрования, целое, 64 бит, big-endian
    key - ключ в виде списка из восьми 32-битных целых чисел'''
    n1, n2 = data64 & 0xffffffff, data64 >> 32
    for i in xrange(24):
        n1, n2 = rotl32(subst((n1 + key[(8 - i - 1) % 8]) % HALF_BLOCK_SIZE, CBRF_SBOX_OPT)) ^ n2, n1
    for i in xrange(24, 32):
        n1, n2 = rotl32(subst((n1 + key[i % 8]) % HALF_BLOCK_SIZE, CBRF_SBOX_OPT)) ^ n2, n1
    return n1 << 32 | n2


def gamma_generator(init_vector64, key):
    '''Генерация гаммы режима гаммирования ГОСТа 28147-89.
    init_vector64 - синхропосылка, 64 бит
    key - ключ в виде списка из восьми 32-битных целых чисел'''
    n = encrypt_ecb(init_vector64, key)
    while True:
        n = ((((n >> 32) + GOST_C1) % HALF_BLOCK_SIZE) << 32) | (((n & 0xffffffff) + GOST_C2) % HALF_BLOCK_SIZE)
        yield encrypt_ecb(n, key)


def encrypt_cfb_generator(init_vector64, key, plain_text):
    '''Шифрование в режиме гаммирования с обратной связью (The Cipher Feedback
    Mode encryption) ГОСТ 28147-89.
    plain_text - список 64-битных целых чисел
    init_vector64 - синхропосылка, 64 бит
    key - ключ в виде списка из восьми 32-битных целых чисел'''
    crypted = init_vector64
    for block_plain64 in plain_text:
        crypted = encrypt_ecb(crypted, key) ^ block_plain64
        yield crypted


class GenerationLogic(object):
    def seed(self, init_vector64_cfb, key_cfb, plain_text):
        u'''Инициализация. Все проверки должны быть уже произведены'''
        self._cfb_generator = encrypt_cfb_generator(init_vector64_cfb, key_cfb, plain_text)

    def random64(self):
        return self._cfb_generator.next()
