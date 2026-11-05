import json,re,pathlib,hashlib,sys
r=pathlib.Path('.')
problems=[]
plan=(r/'plan/PLAN.md').read_text(); env=(r/'plan/ENVIRONMENT.md').read_text(); gates=(r/'plan/GATES.md').read_text()
for f in ['HANDOFF.md','plan/PROFILE.md','plan/PLAN.md','plan/ASSUMPTIONS.md','plan/DECISIONS.md','plan/GATES.md','plan/ENVIRONMENT.md','plan/TRACEABILITY.md','research/QUESTIONS.md','research/PRIOR_KNOWLEDGE.md','research/claims.json','research/SOURCES.md','tests/FROZEN_MANIFEST.sha256','premortem/RISK_REGISTER.md','premortem/ASSUMPTION_AUDIT.md','reviews/CODE_REVIEW.md','reviews/ADVERSARIAL_REVIEW.md','.checkpoints/state.json']:
 p=r/f
 if not p.is_file() or p.stat().st_size==0:problems.append(f'missing/empty {f}')
for sid in ['S-000','S-REVIEW','S-ADVERSARIAL','S-LISTEN','S-RETRO','S-KNOW']:
 if f'### {sid}' not in plan:problems.append(f'missing step {sid}')
if not plan.lstrip().startswith('# READY'):problems.append('not READY')
if 'G-003' not in gates or 'G-LISTEN' not in gates:problems.append('missing required gates')
for bad in ['|| true','continue-on-error','set +e']:
 for f in ['plan/PLAN.md','plan/ENVIRONMENT.md']:
  if bad in (r/f).read_text():problems.append(f'{bad} in {f}')
claims=json.loads((r/'research/claims.json').read_text())
if len(claims)!=14:problems.append('claim count')
for cid in [f'C-{i:03}' for i in range(1,15)]:
 if not any(c['id']==cid for c in claims):problems.append(f'missing {cid}')
for did in [f'D-{i:03}' for i in range(1,26)]:
 if did not in (r/'plan/DECISIONS.md').read_text():problems.append(f'missing {did}')
for tid in [f'T{i:03}' for i in range(1,29)]:
 if tid not in plan or tid not in (r/'plan/TRACEABILITY.md').read_text():problems.append(f'unmapped {tid}')
for label in ['ubuntu-24.04','windows-2025','macos-15-intel','macos-15']:
 if label not in env:problems.append(f'missing runner {label}')
for line in (r/'tests/FROZEN_MANIFEST.sha256').read_text().splitlines():
 h,f=line.split(maxsplit=1);p=r/f.strip()
 if not p.is_file() or hashlib.sha256(p.read_bytes()).hexdigest()!=h:problems.append(f'freeze mismatch {f}')
for p in list(r.glob('lessons/L-*.md'))+list(r.glob('knowledge/K-*.md')):
 text=p.read_text()
 if re.search(r'github_pat_[A-Za-z0-9_]{20,}|ghp_[A-Za-z0-9]{20,}',text):problems.append(f'secret {p}')
 for field in (['id','title','status','supersedes','recurrence_of','occurrences','severity','tags','recorded_by','run','recorded_at'] if p.name.startswith('L-') else ['id','statement','status','supersedes','tags','applies_to_version','source','confidence','derived_from','discovered_by','run','recorded_at']):
  if not re.search(rf'^{field}:',text,re.M):problems.append(f'{p} missing {field}')
print('\n'.join(problems) if problems else 'PASS: zero mechanical cold-read items')
sys.exit(bool(problems))
