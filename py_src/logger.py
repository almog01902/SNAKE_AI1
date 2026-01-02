import sys

class Logger(object):
    def __init__(self, filename="snake_inference_log.txt"):
        """
        מחלקת לוגר שכותבת גם למסך וגם לקובץ חיצוני.
        :param filename: שם הקובץ אליו יישמר הלוג.
        """
        self.terminal = sys.stdout
        # משתמשים ב-"w" כדי שהקובץ יתאפס בכל הרצה חדשה. 
        # אם תרצה להוסיף לסוף קובץ קיים, שנה ל-"a".
        self.log = open(filename, "w", encoding="utf-8")

    def write(self, message):
        self.terminal.write(message)
        self.log.write(message)

    def flush(self):
        # נחוץ עבור תאימות לפייתון ומערכות הפעלה מסוימות
        self.terminal.flush()
        self.log.flush()

def setup_logger(filename="snake_inference_log.txt"):
    """פונקציית עזר להפעלת הלוגר בקובץ הראשי"""
    sys.stdout = Logger(filename)