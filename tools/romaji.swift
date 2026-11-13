import Foundation
// Reads lines of Japanese text, writes each one's reading in Latin letters, tokenised as Japanese.
let locale = Locale(identifier: "ja") as CFLocale
while let line = readLine(strippingNewline: true) {
    let s = line as NSString
    let tok = CFStringTokenizerCreate(nil, s, CFRangeMake(0, s.length), kCFStringTokenizerUnitWordBoundary, locale)
    var out: [String] = []
    while CFStringTokenizerAdvanceToNextToken(tok) != [] {
        let r = CFStringTokenizerGetCurrentTokenRange(tok)
        if let latin = CFStringTokenizerCopyCurrentTokenAttribute(tok, kCFStringTokenizerAttributeLatinTranscription) as? String {
            out.append(latin)
        } else {
            out.append(s.substring(with: NSRange(location: r.location, length: r.length)))
        }
    }
    print(out.joined(separator: " "))
}
