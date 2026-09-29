import hashlib,json,pathlib,subprocess,tempfile,unittest
BASE=pathlib.Path(__file__).resolve().parents[1];ENGINE=BASE/'runtime/logharbor'
def fixture(text,*args):
 with tempfile.TemporaryDirectory() as d:
  p=pathlib.Path(d)/'log.txt';p.write_text(text);before=hashlib.sha256(p.read_bytes()).hexdigest();r=subprocess.run([str(ENGINE),str(p),*args],capture_output=True,text=True,check=True);assert before==hashlib.sha256(p.read_bytes()).hexdigest();return json.loads(r.stdout)
class LogHarborTests(unittest.TestCase):
 def test_three_formats(self):
  r=fixture((BASE/'fixtures/service-mixed.log').read_text());self.assertEqual(r['parsed'],13);self.assertEqual(r['malformed'],2);self.assertEqual(len(r['formats']),3)
 def test_timezone_crosses_day(self):
  r=fixture('2026-01-01T00:30:00+02:00 ERROR rollover\n2026-01-01T23:30:00-0200 INFO nextday\n');self.assertEqual(r['records'][0]['timestamp'],'2025-12-31T22:30:00Z');self.assertEqual(r['records'][1]['timestamp'],'2026-01-02T01:30:00Z')
 def test_invalid_dates_and_zone(self):
  r=fixture('2026-02-29T00:00:00Z INFO bad\n2026-09-28T00:00:00+25:00 ERROR bad\n2024-02-29T00:00:00Z INFO valid\n');self.assertEqual(r['parsed'],1);self.assertEqual(r['malformed'],2)
 def test_filters_case_insensitive(self):
  r=fixture('2026-09-28T01:00:00Z INFO timeout\n2026-09-28T01:00:01Z ERROR TIMEOUT upstream\n2026-09-28T01:00:02Z ERROR other\n','ERROR','timeout');self.assertEqual(r['matched'],1);self.assertEqual(r['records'][0]['line'],2);self.assertEqual(r['parsed'],3)
 def test_report_memory_cap(self):
  r=fixture('2026-09-28T00:00:00Z INFO repeated\n'*25000);self.assertEqual(r['lines'],25000);self.assertEqual(r['matched'],25000);self.assertEqual(len(r['records']),500)
 def test_oversized_line(self):
  r=fixture('x'*70000+'\n2026-09-28T00:00:00Z INFO recovered\n');self.assertEqual(r['malformed'],1);self.assertEqual(r['parsed'],1);self.assertIn('64 KiB',r['diagnostics'][0]['reason'])
 def test_empty_file(self):self.assertEqual(fixture('')['lines'],0)
 def test_crlf_and_unicode_json(self):
  r=fixture('2026-09-28T00:00:00Z INFO quote "東京"\r\n');self.assertEqual(r['records'][0]['message'],'quote "東京"')
 def test_syslog_year_and_priority(self):
  r=fixture('Jan  2 03:04:05 host daemon[2]: fatal out of memory\n','DEBUG','','2025');self.assertEqual(r['records'][0]['timestamp'],'2025-01-02T03:04:05Z');self.assertEqual(r['records'][0]['level'],'CRITICAL')
 def test_apache_http_status(self):
  r=fixture('192.0.2.1 - - [28/Sep/2026:09:00:00 +0200] "GET /lost HTTP/1.1" 404 1\n');self.assertEqual(r['records'][0]['level'],'WARNING')
 def test_invalid_argument(self):
  r=subprocess.run([str(ENGINE),str(BASE/'fixtures/service-mixed.log'),'NOPE'],capture_output=True);self.assertEqual(r.returncode,2)
 def test_nonexistent(self):
  r=subprocess.run([str(ENGINE),str(BASE/'missing')],capture_output=True);self.assertEqual(r.returncode,2)
if __name__=='__main__':unittest.main(verbosity=2)
