'use strict';
const fs = require('fs');
const path = require('path');

const transcript = path.join(
  process.env.USERPROFILE || '',
  '.cursor/projects/c-Team-FightGame-FightGame/agent-transcripts/861678d9-db93-49b4-af0d-8ab59c60320a/861678d9-db93-49b4-af0d-8ab59c60320a.jsonl'
);
const outPath = path.join(__dirname, '../../docs/ai-behavior.md');
const memoPath = path.join(__dirname, '../../docs/ai-behavior-memo.md');

const lines = fs.readFileSync(transcript, 'utf8').split(/\r?\n/);
let best = '';

for (const line of lines) {
  if (!line) continue;
  try {
    const o = JSON.parse(line);
    const parts = o.message && o.message.content;
    if (!Array.isArray(parts)) continue;
    for (const c of parts) {
      if (c.type !== 'tool_use' || c.name !== 'Write') continue;
      const contents = c.input && c.input.contents;
      const p = c.input && c.input.path;
      if (!contents || !contents.startsWith('# \u6575AI\u306E\u632F\u308B\u821E\u3044')) continue;
      if (!p || !String(p).includes('ai-behavior.md')) continue;
      if (contents.length > best.length) best = contents;
    }
  } catch (_) {
    /* skip bad lines */
  }
}

if (!best) {
  console.error('ai-behavior source not found in transcript');
  process.exit(1);
}

fs.writeFileSync(outPath, best.replace(/\r?\n/g, '\r\n'), 'utf8');
if (fs.existsSync(memoPath)) fs.unlinkSync(memoPath);

const t = fs.readFileSync(outPath, 'utf8');
console.log('ok', outPath, fs.statSync(outPath).size, 'fffd', t.includes('\uFFFD'), 'lines', t.split(/\r?\n/).length);
