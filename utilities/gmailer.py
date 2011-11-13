#!/usr/bin/env python3
import argparse
import sys
import smtplib
import os
import configparser
import email.mime.text

def gmailer(user, pw, from_email, to_email, subject, body, debug=False):
	"""
	Connects to Gmail SMTP server and sends email
	"""
	# Construct message
	msg = email.mime.text.MIMEText(body)
	msg['Subject'] = subject
	msg['From'] = from_email
	msg['To'] = ', '.join(to_email)
	
	# Connect
	smtp = smtplib.SMTP_SSL()
	if debug:
		smtp.set_debuglevel(1)
	
	smtp.connect(host='smtp.gmail.com', port=465)
	smtp.login(user, pw)
	
	# Send
	smtp.sendmail(from_email, to_email, msg.as_string())
	smtp.quit()

def main(argv = None):
	parser = argparse.ArgumentParser(description='Sends an email through Gmail')
	parser.add_argument('--debug', action='store_true', help='Print detailed debugging information for sends')
	parser.add_argument('--batch', action='store_true', help='Disable all output')
	parser.add_argument('--from', '-f', dest='from_email', help='Who the email is from')
	parser.add_argument('--to', '-t', action='append', help='Who to send the email to')
	parser.add_argument('--subject', '-s', help='Email subject')
	parser.add_argument('--body', '-b', help='Text of email. If this or --body-file not given, message must be entered on stdin')
	parser.add_argument('--body-file', help='File to use for email body. Overrides --body')
	parser.add_argument('--config', '-c', default='~/.gmailer', help='Configuration file with Gmail login credentials. Defaults to ~/.gmailer')
	args = parser.parse_args()
	
	# Open config file and read in those values
	try:
		config = configparser.ConfigParser()
		config.read(os.path.expanduser(args.config))
		username = config['auth']['user']
		pw = config['auth']['password']
	except KeyError as e:
		if os.path.exists(args.config):
			print('Error in configuration file:', e)
		else:
			print('Ensure configure file', args.config, 'exists')
		sys.exit(1)
		
	# Get the arguments the user didn't give on command line
	if args.from_email is None:
		args.from_email = input('From: ')
		
	if args.to is None:
		args.to = [input('To: ')]
	
	if args.subject is None:
		args.subject = input('Subject: ')
	
	if args.body_file is not None:
		# Read file in as body
		with open(os.path.expanduser(args.body_file), 'r') as f:
			args.body = f.read()
	
	if args.body is None:
		# Not on command line and no file given
		if not args.batch:
			print('Body: (Ctrl-D twice to end... Python bug, sorry)')
		args.body = sys.stdin.read()
	
	# And attempt to send
	try:
		if not args.batch:
			print('Sending...')
			
		gmailer(username, pw, args.from_email, args.to, args.subject, args.body, debug=args.debug)
		
		if not args.batch:
			print('Email sent')
			
		return 0
	except Exception as e:
		if not args.batch:
			print('Email failed:', e)
			
		return 2	

if __name__ == '__main__':
	sys.exit(main(sys.argv))
	
