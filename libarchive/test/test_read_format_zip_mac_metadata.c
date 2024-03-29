/*-
 * Copyright (c) 2012 Michihiro NAKAJIMA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "test.h"

/*
 * Read a zip file that has a zip comment in the end of the central
 * directory record.
 */
DEFINE_TEST(test_read_format_zip_mac_metadata)
{
	const char *refname = "test_read_format_zip_mac_metadata.zip";
	char *p;
	size_t s;
	struct archive *a;
	struct archive_entry *ae;
	const unsigned char appledouble[] = {
		0x00, 0x05, 0x16, 0x07, 0x00, 0x02, 0x00, 0x00,
		0x4d, 0x61, 0x63, 0x20, 0x4f, 0x53, 0x20, 0x58,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
		0x00, 0x02, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
		0x00, 0x32, 0x00, 0x00, 0x00, 0xed, 0x00, 0x00,
		0x00, 0x02, 0x00, 0x00, 0x01, 0x1f, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x41, 0x54, 0x54, 0x52,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1f,
		0x00, 0x00, 0x00, 0x98, 0x00, 0x00, 0x00, 0x87,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x98, 0x00, 0x00, 0x00, 0x87,
		0x00, 0x00, 0x13, 0x63, 0x6f, 0x6d, 0x2e, 0x61,
		0x70, 0x70, 0x6c, 0x65, 0x2e, 0x61, 0x63, 0x6c,
		0x2e, 0x74, 0x65, 0x78, 0x74, 0x00, 0x00, 0x00,
		0x21, 0x23, 0x61, 0x63, 0x6c, 0x20, 0x31, 0x0a,
		0x75, 0x73, 0x65, 0x72, 0x3a, 0x46, 0x46, 0x46,
		0x46, 0x45, 0x45, 0x45, 0x45, 0x2d, 0x44, 0x44,
		0x44, 0x44, 0x2d, 0x43, 0x43, 0x43, 0x43, 0x2d,
		0x42, 0x42, 0x42, 0x42, 0x2d, 0x41, 0x41, 0x41,
		0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x43,
		0x39, 0x3a, 0x47, 0x75, 0x65, 0x73, 0x74, 0x3a,
		0x32, 0x30, 0x31, 0x3a, 0x64, 0x65, 0x6e, 0x79,
		0x3a, 0x72, 0x65, 0x61, 0x64, 0x0a, 0x67, 0x72,
		0x6f, 0x75, 0x70, 0x3a, 0x41, 0x42, 0x43, 0x44,
		0x45, 0x46, 0x41, 0x42, 0x2d, 0x43, 0x44, 0x45,
		0x46, 0x2d, 0x41, 0x42, 0x43, 0x44, 0x2d, 0x45,
		0x46, 0x41, 0x42, 0x2d, 0x43, 0x44, 0x45, 0x46,
		0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x35, 0x30,
		0x3a, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x3a, 0x38,
		0x30, 0x3a, 0x61, 0x6c, 0x6c, 0x6f, 0x77, 0x3a,
		0x77, 0x72, 0x69, 0x74, 0x65, 0x0a, 0x00
	};

	extract_reference_file(refname);
	p = slurpfile(&s, "%s", refname);

	/* Mac metadata can only be extracted with the seeking reader. */
	assert((a = archive_read_new()) != NULL);
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_set_option(a, "zip", "mac-ext", "1"));
	assertEqualIntA(a, ARCHIVE_OK, read_open_memory_seek(a, p, s, 1));

	if (archive_zlib_version() != NULL) {
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_next_header(a, &ae));
	} else {
		assertEqualIntA(a, ARCHIVE_WARN,
		    archive_read_next_header(a, &ae));
		assertEqualString(archive_error_string(a),
		    "Unsupported ZIP compression method (deflation)");
		assert(archive_errno(a) != 0);
	}
	assertEqualInt(archive_entry_is_encrypted(ae), 0);
	assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);
	assertEqualString("file3", archive_entry_pathname(ae));
	assertEqualInt(AE_IFREG | 0644, archive_entry_mode(ae));
	failure("Mac metadata should be set");
	if (archive_zlib_version() != NULL) {
		const void *metadata;
		if (assert((metadata = archive_entry_mac_metadata(ae, &s))
		    != NULL)) {
			assertEqualMem(metadata, appledouble,
			    sizeof(appledouble));
		}
	} else {
		assert(archive_entry_mac_metadata(ae, &s) == NULL);
	}
	assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));

	free(p);
}
