/**
 * Copyright 2020-2021 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "client_test_helpers.h"
#include "sa.h"
#include "sa_key_unwrap_common.h"
#include "gtest/gtest.h"

using namespace client_test_helpers;

namespace {
    TEST_F(SaKeyUnwrapAesGcmTest, failsNullKey) {
        std::vector<uint8_t> clear_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key;
        std::vector<uint8_t> clear_wrapping_key;
        std::shared_ptr<void> wrapping_parameters;
        std::vector<uint8_t> wrapped_key;
        ASSERT_TRUE(wrap_key(wrapping_key, clear_wrapping_key, wrapped_key, wrapping_parameters,
                SYM_128_KEY_SIZE, clear_key, SA_CIPHER_ALGORITHM_AES_GCM));

        sa_rights rights;
        rights_set_allow_all(&rights);

        sa_status status = sa_key_unwrap(nullptr, &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, wrapping_parameters.get(), *wrapping_key,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_NULL_PARAMETER);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsNullRights) {
        std::vector<uint8_t> clear_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key;
        std::vector<uint8_t> clear_wrapping_key;
        std::shared_ptr<void> wrapping_parameters;
        std::vector<uint8_t> wrapped_key;
        ASSERT_TRUE(wrap_key(wrapping_key, clear_wrapping_key, wrapped_key, wrapping_parameters,
                SYM_128_KEY_SIZE, clear_key, SA_CIPHER_ALGORITHM_AES_GCM));

        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), nullptr, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, wrapping_parameters.get(), *wrapping_key,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_NULL_PARAMETER);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsNullIn) {
        std::vector<uint8_t> clear_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key;
        std::vector<uint8_t> clear_wrapping_key;
        std::shared_ptr<void> wrapping_parameters;
        std::vector<uint8_t> wrapped_key;
        ASSERT_TRUE(wrap_key(wrapping_key, clear_wrapping_key, wrapped_key, wrapping_parameters,
                SYM_128_KEY_SIZE, clear_key, SA_CIPHER_ALGORITHM_AES_GCM));

        sa_rights rights;
        rights_set_allow_all(&rights);

        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, wrapping_parameters.get(), *wrapping_key,
                nullptr, 0);
        ASSERT_EQ(status, SA_STATUS_NULL_PARAMETER);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsNullAlgorithmParameters) {
        std::vector<uint8_t> clear_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key;
        std::vector<uint8_t> clear_wrapping_key;
        std::shared_ptr<void> wrapping_parameters;
        std::vector<uint8_t> wrapped_key;
        ASSERT_TRUE(wrap_key(wrapping_key, clear_wrapping_key, wrapped_key, wrapping_parameters,
                SYM_128_KEY_SIZE, clear_key, SA_CIPHER_ALGORITHM_AES_GCM));

        sa_rights rights;
        rights_set_allow_all(&rights);

        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, nullptr, *wrapping_key, wrapped_key.data(),
                wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_NULL_PARAMETER);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsNullIv) {
        std::vector<uint8_t> wrapped_key = random(AES_BLOCK_SIZE);

        sa_rights rights;
        rights_set_allow_all(&rights);
        std::vector<uint8_t> clear_wrapping_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key = create_sa_key_symmetric(&rights, clear_wrapping_key);
        ASSERT_NE(wrapping_key, nullptr);

        std::vector<uint8_t> aad = random(1024);
        std::vector<uint8_t> tag = random(AES_BLOCK_SIZE);
        sa_unwrap_parameters_aes_gcm unwrap_parameters_aes_gcm = {
                .iv = nullptr,
                .iv_length = GCM_IV_LENGTH,
                .aad = aad.data(),
                .aad_length = aad.size(),
                .tag = tag.data(),
                .tag_length = tag.size()};

        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, &unwrap_parameters_aes_gcm, *wrapping_key,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_NULL_PARAMETER);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsInvalidIv) {
        std::vector<uint8_t> wrapped_key = random(AES_BLOCK_SIZE);

        sa_rights rights;
        rights_set_allow_all(&rights);
        std::vector<uint8_t> clear_wrapping_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key = create_sa_key_symmetric(&rights, clear_wrapping_key);
        ASSERT_NE(wrapping_key, nullptr);

        std::vector<uint8_t> iv = random(GCM_IV_LENGTH + 1);
        std::vector<uint8_t> aad = random(1024);
        std::vector<uint8_t> tag = random(AES_BLOCK_SIZE);
        sa_unwrap_parameters_aes_gcm unwrap_parameters_aes_gcm = {
                .iv = iv.data(),
                .iv_length = iv.size(),
                .aad = aad.data(),
                .aad_length = aad.size(),
                .tag = tag.data(),
                .tag_length = tag.size()};

        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, &unwrap_parameters_aes_gcm, *wrapping_key,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_BAD_PARAMETER);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsNullAad) {
        std::vector<uint8_t> wrapped_key = random(AES_BLOCK_SIZE);

        sa_rights rights;
        rights_set_allow_all(&rights);
        std::vector<uint8_t> clear_wrapping_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key = create_sa_key_symmetric(&rights, clear_wrapping_key);
        ASSERT_NE(wrapping_key, nullptr);

        std::vector<uint8_t> iv = random(GCM_IV_LENGTH);
        std::vector<uint8_t> tag = random(AES_BLOCK_SIZE);
        sa_unwrap_parameters_aes_gcm unwrap_parameters_aes_gcm = {
                .iv = iv.data(),
                .iv_length = iv.size(),
                .aad = nullptr,
                .aad_length = 1,
                .tag = tag.data(),
                .tag_length = tag.size()};

        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, &unwrap_parameters_aes_gcm, *wrapping_key,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_NULL_PARAMETER);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsNullTag) {
        std::vector<uint8_t> wrapped_key = random(AES_BLOCK_SIZE);

        sa_rights rights;
        rights_set_allow_all(&rights);
        std::vector<uint8_t> clear_wrapping_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key = create_sa_key_symmetric(&rights, clear_wrapping_key);
        ASSERT_NE(wrapping_key, nullptr);

        std::vector<uint8_t> iv = random(GCM_IV_LENGTH);
        std::vector<uint8_t> aad = random(1024);
        sa_unwrap_parameters_aes_gcm unwrap_parameters_aes_gcm = {
                .iv = iv.data(),
                .iv_length = iv.size(),
                .aad = aad.data(),
                .aad_length = aad.size(),
                .tag = nullptr,
                .tag_length = 0};

        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, &unwrap_parameters_aes_gcm, *wrapping_key,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_NULL_PARAMETER);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsInvalidTag) {
        std::vector<uint8_t> wrapped_key = random(AES_BLOCK_SIZE);

        sa_rights rights;
        rights_set_allow_all(&rights);
        std::vector<uint8_t> clear_wrapping_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key = create_sa_key_symmetric(&rights, clear_wrapping_key);
        ASSERT_NE(wrapping_key, nullptr);

        std::vector<uint8_t> iv = random(GCM_IV_LENGTH);
        std::vector<uint8_t> aad = random(1024);
        std::vector<uint8_t> tag = random(AES_BLOCK_SIZE + 1);
        sa_unwrap_parameters_aes_gcm unwrap_parameters_aes_gcm = {
                .iv = iv.data(),
                .iv_length = iv.size(),
                .aad = aad.data(),
                .aad_length = aad.size(),
                .tag = tag.data(),
                .tag_length = tag.size()};

        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, &unwrap_parameters_aes_gcm, *wrapping_key,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_BAD_PARAMETER);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsUnknownWrappingKey) {
        std::vector<uint8_t> clear_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key;
        std::vector<uint8_t> clear_wrapping_key;
        std::shared_ptr<void> wrapping_parameters;
        std::vector<uint8_t> wrapped_key;
        ASSERT_TRUE(wrap_key(wrapping_key, clear_wrapping_key, wrapped_key, wrapping_parameters,
                SYM_128_KEY_SIZE, clear_key, SA_CIPHER_ALGORITHM_AES_GCM));

        sa_rights rights;
        rights_set_allow_all(&rights);

        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, wrapping_parameters.get(), INVALID_HANDLE,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_BAD_PARAMETER);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsWrappingKeyDisallowsUnwrap) {
        std::vector<uint8_t> wrapped_key = random(AES_BLOCK_SIZE);

        sa_rights wrapping_key_rights;
        rights_set_allow_all(&wrapping_key_rights);
        SA_USAGE_BIT_CLEAR(wrapping_key_rights.usage_flags, SA_USAGE_FLAG_UNWRAP);
        std::vector<uint8_t> clear_wrapping_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key = create_sa_key_symmetric(&wrapping_key_rights, clear_wrapping_key);
        ASSERT_NE(wrapping_key, nullptr);

        std::vector<uint8_t> iv = random(GCM_IV_LENGTH);
        std::vector<uint8_t> aad = random(1024);
        std::vector<uint8_t> tag = random(AES_BLOCK_SIZE);
        sa_unwrap_parameters_aes_gcm unwrap_parameters_aes_gcm = {
                .iv = iv.data(),
                .iv_length = iv.size(),
                .aad = aad.data(),
                .aad_length = aad.size(),
                .tag = tag.data(),
                .tag_length = tag.size()};

        sa_rights rights;
        rights_set_allow_all(&rights);
        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, &unwrap_parameters_aes_gcm, *wrapping_key,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_OPERATION_NOT_ALLOWED);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsWrappingKeyOutsideValidTimeBefore) {
        std::vector<uint8_t> wrapped_key = random(AES_BLOCK_SIZE);

        sa_rights wrapping_key_rights;
        rights_set_allow_all(&wrapping_key_rights);
        wrapping_key_rights.not_before = time(nullptr) + 60;
        std::vector<uint8_t> clear_wrapping_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key = create_sa_key_symmetric(&wrapping_key_rights, clear_wrapping_key);
        ASSERT_NE(wrapping_key, nullptr);

        std::vector<uint8_t> iv = random(GCM_IV_LENGTH);
        std::vector<uint8_t> aad = random(1024);
        std::vector<uint8_t> tag = random(AES_BLOCK_SIZE);
        sa_unwrap_parameters_aes_gcm unwrap_parameters_aes_gcm = {
                .iv = iv.data(),
                .iv_length = iv.size(),
                .aad = aad.data(),
                .aad_length = aad.size(),
                .tag = tag.data(),
                .tag_length = tag.size()};

        sa_rights rights;
        rights_set_allow_all(&rights);
        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, &unwrap_parameters_aes_gcm, *wrapping_key,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_OPERATION_NOT_ALLOWED);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsWrappingKeyOutsideValidTimeAfter) {
        std::vector<uint8_t> wrapped_key = random(AES_BLOCK_SIZE);

        sa_rights wrapping_key_rights;
        rights_set_allow_all(&wrapping_key_rights);
        wrapping_key_rights.not_on_or_after = time(nullptr) - 60;
        std::vector<uint8_t> clear_wrapping_key = random(SYM_128_KEY_SIZE);
        std::shared_ptr<sa_key> wrapping_key = create_sa_key_symmetric(&wrapping_key_rights, clear_wrapping_key);
        ASSERT_NE(wrapping_key, nullptr);

        std::vector<uint8_t> iv = random(GCM_IV_LENGTH);
        std::vector<uint8_t> aad = random(1024);
        std::vector<uint8_t> tag = random(AES_BLOCK_SIZE);
        sa_unwrap_parameters_aes_gcm unwrap_parameters_aes_gcm = {
                .iv = iv.data(),
                .iv_length = iv.size(),
                .aad = aad.data(),
                .aad_length = aad.size(),
                .tag = tag.data(),
                .tag_length = tag.size()};

        sa_rights rights;
        rights_set_allow_all(&rights);
        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, &unwrap_parameters_aes_gcm, *wrapping_key,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_OPERATION_NOT_ALLOWED);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsWrappingKeyNotAes) {
        auto curve = SA_ELLIPTIC_CURVE_NIST_P256;
        auto key_size = ec_get_key_size(curve);
        std::vector<uint8_t> wrapped_key = random_ec(AES_BLOCK_SIZE);

        sa_rights rights;
        rights_set_allow_all(&rights);
        std::vector<uint8_t> clear_wrapping_key = random(key_size);
        std::shared_ptr<sa_key> wrapping_key = create_sa_key_ec(&rights, curve, clear_wrapping_key);
        ASSERT_NE(wrapping_key, nullptr);

        std::vector<uint8_t> iv = random(GCM_IV_LENGTH);
        std::vector<uint8_t> aad = random(1024);
        std::vector<uint8_t> tag = random(AES_BLOCK_SIZE);
        sa_unwrap_parameters_aes_gcm unwrap_parameters_aes_gcm = {
                .iv = iv.data(),
                .iv_length = iv.size(),
                .aad = aad.data(),
                .aad_length = aad.size(),
                .tag = tag.data(),
                .tag_length = tag.size()};

        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, &unwrap_parameters_aes_gcm, *wrapping_key,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_BAD_KEY_TYPE);
    }

    TEST_F(SaKeyUnwrapAesGcmTest, failsWrappingKeyNotValidAesSize) {
        std::vector<uint8_t> wrapped_key = random(AES_BLOCK_SIZE);

        sa_rights rights;
        rights_set_allow_all(&rights);
        std::vector<uint8_t> clear_wrapping_key = random(SYM_128_KEY_SIZE + 1);
        std::shared_ptr<sa_key> wrapping_key = create_sa_key_symmetric(&rights, clear_wrapping_key);
        ASSERT_NE(wrapping_key, nullptr);

        std::vector<uint8_t> iv = random(GCM_IV_LENGTH);
        std::vector<uint8_t> aad = random(1024);
        std::vector<uint8_t> tag = random(AES_BLOCK_SIZE);
        sa_unwrap_parameters_aes_gcm unwrap_parameters_aes_gcm = {
                .iv = iv.data(),
                .iv_length = iv.size(),
                .aad = aad.data(),
                .aad_length = aad.size(),
                .tag = tag.data(),
                .tag_length = tag.size()};

        auto unwrapped_key = create_uninitialized_sa_key();
        ASSERT_NE(unwrapped_key, nullptr);
        sa_status status = sa_key_unwrap(unwrapped_key.get(), &rights, SA_KEY_TYPE_SYMMETRIC, nullptr,
                SA_CIPHER_ALGORITHM_AES_GCM, &unwrap_parameters_aes_gcm, *wrapping_key,
                wrapped_key.data(), wrapped_key.size());
        ASSERT_EQ(status, SA_STATUS_BAD_KEY_TYPE);
    }
} // namespace
