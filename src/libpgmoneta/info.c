/*
 * Copyright (C) 2024 The pgmoneta community
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list
 * of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may
 * be used to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* pgmoneta */
#include <pgmoneta.h>
#include <info.h>
#include <logging.h>
#include <management.h>
#include <utils.h>

/* system */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INFO_BUFFER_SIZE 8192

void
pgmoneta_create_info(char* directory, char* label, int status)
{
   char buffer[INFO_BUFFER_SIZE];
   char* s = NULL;
   FILE* sfile = NULL;

   s = pgmoneta_append(s, directory);
   s = pgmoneta_append(s, "/backup.info");

   sfile = fopen(s, "w");

   memset(&buffer[0], 0, sizeof(buffer));
   snprintf(&buffer[0], sizeof(buffer), "%s=%d\n", INFO_STATUS, status);
   fputs(&buffer[0], sfile);

   memset(&buffer[0], 0, sizeof(buffer));
   snprintf(&buffer[0], sizeof(buffer), "%s=%s\n", INFO_LABEL, label);
   fputs(&buffer[0], sfile);

   memset(&buffer[0], 0, sizeof(buffer));
   snprintf(&buffer[0], sizeof(buffer), "%s=0\n", INFO_TABLESPACES);
   fputs(&buffer[0], sfile);

   memset(&buffer[0], 0, sizeof(buffer));
   snprintf(&buffer[0], sizeof(buffer), "%s=%s\n", INFO_PGMONETA_VERSION, VERSION);
   fputs(&buffer[0], sfile);

   memset(&buffer[0], 0, sizeof(buffer));
   snprintf(&buffer[0], sizeof(buffer), "%s=\n", INFO_COMMENTS);
   fputs(&buffer[0], sfile);

   pgmoneta_log_trace("%s=%d", INFO_STATUS, status);
   pgmoneta_log_trace("%s=%s", INFO_LABEL, label);
   pgmoneta_log_trace("%s=0", INFO_TABLESPACES);
   pgmoneta_log_trace("%s=%s", INFO_PGMONETA_VERSION, VERSION);
   pgmoneta_log_trace("%s=", INFO_COMMENTS);
   pgmoneta_log_trace("%s=", INFO_EXTRA);

   pgmoneta_permission(s, 6, 0, 0);

   if (sfile != NULL)
   {
      fclose(sfile);
   }

   free(s);
}

void
pgmoneta_update_info_unsigned_long(char* directory, char* key, unsigned long value)
{
   char buffer[INFO_BUFFER_SIZE];
   char line[INFO_BUFFER_SIZE];
   bool found = false;
   char* s = NULL;
   FILE* sfile = NULL;
   char* d = NULL;
   FILE* dfile = NULL;

   s = pgmoneta_append(s, directory);
   s = pgmoneta_append(s, "/backup.info");

   d = pgmoneta_append(d, directory);
   d = pgmoneta_append(d, "/backup.info.tmp");

   sfile = fopen(s, "r");
   dfile = fopen(d, "w");

   while ((fgets(&buffer[0], sizeof(buffer), sfile)) != NULL)
   {
      char k[INFO_BUFFER_SIZE];
      char v[INFO_BUFFER_SIZE];
      char* ptr = NULL;

      memset(&k[0], 0, sizeof(k));
      memset(&v[0], 0, sizeof(v));

      memset(&line[0], 0, sizeof(line));
      memcpy(&line[0], &buffer[0], strlen(&buffer[0]));

      ptr = strtok(&buffer[0], "=");
      memcpy(&k[0], ptr, strlen(ptr));

      ptr = strtok(NULL, "=");
      memcpy(&v[0], ptr, strlen(ptr) - 1);

      if (!strcmp(key, &k[0]))
      {
         memset(&line[0], 0, sizeof(line));
         snprintf(&line[0], sizeof(line), "%s=%lu\n", key, value);
         fputs(&line[0], dfile);
         found = true;
      }
      else
      {
         fputs(&line[0], dfile);
      }
   }

   if (!found)
   {
      memset(&line[0], 0, sizeof(line));
      pgmoneta_log_trace("%s=%lu", key, value);
      snprintf(&line[0], sizeof(line), "%s=%lu\n", key, value);
      fputs(&line[0], dfile);
   }

   if (sfile != NULL)
   {
      fclose(sfile);
   }

   if (dfile != NULL)
   {
      fclose(dfile);
   }

   pgmoneta_move_file(d, s);
   pgmoneta_permission(s, 6, 0, 0);

   free(s);
   free(d);
}

void
pgmoneta_update_info_string(char* directory, char* key, char* value)
{
   char buffer[INFO_BUFFER_SIZE];
   char line[INFO_BUFFER_SIZE];
   bool found = false;
   char* s = NULL;
   FILE* sfile = NULL;
   char* d = NULL;
   FILE* dfile = NULL;

   s = pgmoneta_append(s, directory);
   s = pgmoneta_append(s, "/backup.info");

   d = pgmoneta_append(d, directory);
   d = pgmoneta_append(d, "/backup.info.tmp");

   sfile = fopen(s, "r");
   dfile = fopen(d, "w");

   while ((fgets(&buffer[0], sizeof(buffer), sfile)) != NULL)
   {
      char k[INFO_BUFFER_SIZE];
      char v[INFO_BUFFER_SIZE];
      char* ptr = NULL;

      memset(&k[0], 0, sizeof(k));
      memset(&v[0], 0, sizeof(v));

      memset(&line[0], 0, sizeof(line));
      memcpy(&line[0], &buffer[0], strlen(&buffer[0]));

      ptr = strtok(&buffer[0], "=");
      memcpy(&k[0], ptr, strlen(ptr));

      ptr = strtok(NULL, "=");
      memcpy(&v[0], ptr, strlen(ptr) - 1);

      if (!strcmp(key, &k[0]))
      {
         memset(&line[0], 0, sizeof(line));
         snprintf(&line[0], sizeof(line), "%s=%s\n", key, value);
         fputs(&line[0], dfile);
         found = true;
      }
      else
      {
         fputs(&line[0], dfile);
      }
   }

   if (!found)
   {
      memset(&line[0], 0, sizeof(line));
      pgmoneta_log_trace("%s=%s", key, value);
      snprintf(&line[0], sizeof(line), "%s=%s\n", key, value);
      fputs(&line[0], dfile);
   }

   if (sfile != NULL)
   {
      fclose(sfile);
   }

   if (dfile != NULL)
   {
      fclose(dfile);
   }

   pgmoneta_move_file(d, s);
   pgmoneta_permission(s, 6, 0, 0);

   free(s);
   free(d);
}

void
pgmoneta_update_info_bool(char* directory, char* key, bool value)
{
   pgmoneta_log_trace("%s=%d", key, value ? 1 : 0);
   pgmoneta_update_info_unsigned_long(directory, key, value ? 1 : 0);
}

int
pgmoneta_update_info_annotate(SSL* ssl, int socket, char* server, char* backup, char* command, char* key, char* comment)
{
   char* d = NULL;
   char* dir = NULL;
   char* old_comments = NULL;
   char* new_comments = NULL;
   int srv = -1;
   int number_of_backups = 0;
   bool found = false;
   bool fail = false;
   struct backup** backups = NULL;
   struct backup* bck = NULL;
   struct configuration* config;

   config = (struct configuration*)shmem;

   for (int i = 0; srv == -1 && i < config->number_of_servers; i++)
   {
      if (!strcmp(config->servers[i].name, server))
      {
         srv = i;
      }
   }

   if (srv == -1)
   {
      pgmoneta_log_error("Annotate: No server defined by %s", server);
      goto error;
   }

   d = pgmoneta_get_server_backup(srv);

   if (pgmoneta_get_backups(d, &number_of_backups, &backups))
   {
      goto error;
   }

   if (!strcmp(backup, "oldest"))
   {
      bck = backups[0];
   }
   else if (!strcmp(backup, "latest") || !strcmp(backup, "newest"))
   {
      bck = backups[number_of_backups - 1];
   }
   else
   {
      for (int i = 0; i < number_of_backups; i++)
      {
         if (!strcmp(backups[i]->label, backup))
         {
            bck = backups[i];
         }
      }
   }

   if (bck == NULL)
   {
      pgmoneta_log_error("Annotate: No backup for %s/%s", config->servers[srv].name, backup);
      goto error;
   }

   if (pgmoneta_get_info_string(bck, INFO_COMMENTS, &old_comments))
   {
      goto error;
   }

   if (!strcmp("add", command))
   {
      if (old_comments == NULL || strlen(old_comments) == 0)
      {
         new_comments = pgmoneta_append(new_comments, key);
         new_comments = pgmoneta_append(new_comments, "|");
         new_comments = pgmoneta_append(new_comments, comment);
      }
      else
      {
         char* tokens = NULL;
         char* ptr = NULL;

         tokens = pgmoneta_append(tokens, old_comments);
         ptr = strtok(tokens, ",");

         while (ptr != NULL && !fail)
         {
            char tk[INFO_BUFFER_SIZE];
            char tv[INFO_BUFFER_SIZE];
            char* equal = NULL;

            memset(&tk[0], 0, sizeof(tk));
            memset(&tv[0], 0, sizeof(tv));

            equal = strchr(ptr, '|');

            memcpy(&tk[0], ptr, strlen(ptr) - strlen(equal));
            memcpy(&tv[0], equal + 1, strlen(equal) - 1);

            if (strcmp(key, tk))
            {
               new_comments = pgmoneta_append(new_comments, tk);
               new_comments = pgmoneta_append(new_comments, "|");
               new_comments = pgmoneta_append(new_comments, tv);
               found = true;
            }
            else
            {
               fail = true;
            }

            ptr = strtok(NULL, ",");
            if (ptr != NULL)
            {
               new_comments = pgmoneta_append(new_comments, ",");
            }
         }

         if (!fail)
         {
            new_comments = pgmoneta_append(new_comments, ",");
            new_comments = pgmoneta_append(new_comments, key);
            new_comments = pgmoneta_append(new_comments, "|");
            new_comments = pgmoneta_append(new_comments, comment);
            found = true;
         }

         free(tokens);
      }
   }
   else if (!strcmp("update", command))
   {
      if (old_comments == NULL || strlen(old_comments) == 0)
      {
         fail = true;
      }
      else
      {
         char tokens[INFO_BUFFER_SIZE];
         char* ptr = NULL;

         memset(&tokens[0], 0, sizeof(tokens));
         memcpy(&tokens[0], old_comments, strlen(old_comments));

         ptr = strtok(&tokens[0], ",");

         while (ptr != NULL)
         {
            char tk[INFO_BUFFER_SIZE];
            char tv[INFO_BUFFER_SIZE];
            char* equal = NULL;

            memset(&tk[0], 0, sizeof(tk));
            memset(&tv[0], 0, sizeof(tv));

            equal = strchr(ptr, '|');

            memcpy(&tk[0], ptr, strlen(ptr) - strlen(equal));
            memcpy(&tv[0], equal + 1, strlen(equal) - 1);

            if (strcmp(key, tk))
            {
               new_comments = pgmoneta_append(new_comments, tk);
               new_comments = pgmoneta_append(new_comments, "|");
               new_comments = pgmoneta_append(new_comments, tv);
            }
            else
            {
               new_comments = pgmoneta_append(new_comments, ",");
               new_comments = pgmoneta_append(new_comments, key);
               new_comments = pgmoneta_append(new_comments, "|");
               new_comments = pgmoneta_append(new_comments, comment);
               found = true;
            }

            ptr = strtok(NULL, ",");
            if (ptr != NULL)
            {
               new_comments = pgmoneta_append(new_comments, ",");
            }
         }

         if (!found)
         {
            fail = true;
         }
      }
   }
   else if (!strcmp("remove", command))
   {
      if (old_comments == NULL || strlen(old_comments) == 0)
      {
         fail = true;
      }
      else
      {
         char tokens[INFO_BUFFER_SIZE];
         char* ptr = NULL;

         memset(&tokens[0], 0, sizeof(tokens));
         memcpy(&tokens[0], old_comments, strlen(old_comments));

         ptr = strtok(&tokens[0], ",");

         while (ptr != NULL)
         {
            char tk[INFO_BUFFER_SIZE];
            char tv[INFO_BUFFER_SIZE];
            char* equal = NULL;

            memset(&tk[0], 0, sizeof(tk));
            memset(&tv[0], 0, sizeof(tv));

            equal = strchr(ptr, '|');

            memcpy(&tk[0], ptr, strlen(ptr) - strlen(equal));
            memcpy(&tv[0], equal + 1, strlen(equal) - 1);

            if (strcmp(key, tk))
            {
               new_comments = pgmoneta_append(new_comments, tk);
               new_comments = pgmoneta_append(new_comments, "|");
               new_comments = pgmoneta_append(new_comments, tv);
            }
            else
            {
               found = true;
            }

            ptr = strtok(NULL, ",");
            if (ptr != NULL)
            {
               new_comments = pgmoneta_append(new_comments, ",");
            }
         }

         if (!found)
         {
            fail = true;
         }
      }
   }
   else
   {
      fail = true;
   }

   if (!strcmp(new_comments, ",") || pgmoneta_starts_with(new_comments, ","))
   {
      new_comments = pgmoneta_remove_first(new_comments);

      if (new_comments == NULL)
      {
         fail = true;
      }
   }

   if (pgmoneta_ends_with(new_comments, ","))
   {
      new_comments = pgmoneta_remove_last(new_comments);

      if (new_comments == NULL)
      {
         fail = true;
      }
   }

   if (fail)
   {
      free(new_comments);
      new_comments = NULL;

      if (old_comments != NULL && strlen(old_comments) > 0)
      {
         new_comments = pgmoneta_append(new_comments, old_comments);
      }
   }

   dir = pgmoneta_append(dir, d);
   if (!pgmoneta_ends_with(dir, "/"))
   {
      dir = pgmoneta_append(dir, "/");
   }
   dir = pgmoneta_append(dir, bck->label);
   dir = pgmoneta_append(dir, "/");

   pgmoneta_update_info_string(dir, INFO_COMMENTS, new_comments != NULL && strlen(new_comments) > 0 ? new_comments : "");

   pgmoneta_management_write_annotate(ssl, socket, server, bck->label, new_comments != NULL && strlen(new_comments) > 0 ? new_comments : "");

   for (int i = 0; i < number_of_backups; i++)
   {
      free(backups[i]);
   }
   free(backups);

   free(d);
   free(dir);
   free(old_comments);
   free(new_comments);

   free(server);
   free(backup);
   free(command);
   free(key);
   free(comment);

   return 0;

error:

   pgmoneta_management_write_annotate(ssl, socket, srv != -1 ? server : "Unknown", bck != NULL ? bck->label : "Unknown", "");

   for (int i = 0; i < number_of_backups; i++)
   {
      free(backups[i]);
   }
   free(backups);

   free(d);
   free(dir);
   free(old_comments);
   free(new_comments);

   free(server);
   free(backup);
   free(command);
   free(key);
   free(comment);

   return 1;
}

int
pgmoneta_get_info_string(struct backup* backup, char* key, char** value)
{
   char* result = NULL;

   if (!strcmp(INFO_LABEL, key))
   {
      result = pgmoneta_append(result, backup->label);
   }
   else if (!strcmp(INFO_WAL, key))
   {
      result = pgmoneta_append(result, backup->wal);
   }
   else if (pgmoneta_starts_with(key, "TABLESPACE_OID"))
   {
      unsigned long number = strtoul(key + 14, NULL, 10);

      result = pgmoneta_append(result, backup->tablespaces_oids[number - 1]);
   }
   else if (pgmoneta_starts_with(key, "TABLESPACE_PATH"))
   {
      unsigned long number = strtoul(key + 15, NULL, 10);

      result = pgmoneta_append(result, backup->tablespaces_paths[number - 1]);
   }
   else if (pgmoneta_starts_with(key, "TABLESPACE"))
   {
      unsigned long number = strtoul(key + 10, NULL, 10);

      result = pgmoneta_append(result, backup->tablespaces[number - 1]);
   }
   else if (!strcmp(INFO_COMMENTS, key))
   {
      result = pgmoneta_append(result, backup->comments);
   }
   else
   {
      goto error;
   }

   *value = result;

   return 0;

error:

   return 1;
}

int
pgmoneta_get_backups(char* directory, int* number_of_backups, struct backup*** backups)
{
   char* d = NULL;
   struct backup** bcks = NULL;
   int number_of_directories;
   char** dirs;

   *number_of_backups = 0;
   *backups = NULL;

   number_of_directories = 0;
   dirs = NULL;

   pgmoneta_get_directories(directory, &number_of_directories, &dirs);

   bcks = (struct backup**)malloc(number_of_directories * sizeof(struct backup*));

   if (bcks == NULL)
   {
      goto error;
   }

   memset(bcks, 0, number_of_directories * sizeof(struct backup*));

   for (int i = 0; i < number_of_directories; i++)
   {
      d = pgmoneta_append(d, directory);

      if (pgmoneta_get_backup(d, dirs[i], &bcks[i]))
      {
         goto error;
      }

      free(d);
      d = NULL;
   }

   for (int i = 0; i < number_of_directories; i++)
   {
      free(dirs[i]);
   }
   free(dirs);

   *number_of_backups = number_of_directories;
   *backups = bcks;

   return 0;

error:

   free(d);

   if (dirs != NULL)
   {
      for (int i = 0; i < number_of_directories; i++)
      {
         free(dirs[i]);
      }
      free(dirs);
   }

   return 1;
}

int
pgmoneta_get_backup(char* directory, char* label, struct backup** backup)
{
   char* fn = NULL;
   int ret = 0;

   *backup = NULL;

   fn = NULL;
   fn = pgmoneta_append(fn, directory);
   fn = pgmoneta_append(fn, "/");
   fn = pgmoneta_append(fn, label);
   fn = pgmoneta_append(fn, "/backup.info");

   ret = pgmoneta_get_backup_file(fn, backup);

   free(fn);

   return ret;
}

int
pgmoneta_get_backup_file(char* fn, struct backup** backup)
{
   char buffer[INFO_BUFFER_SIZE];
   FILE* file = NULL;
   int tbl_idx = 0;
   struct backup* bck;

   *backup = NULL;

   file = fopen(fn, "r");

   bck = (struct backup*)malloc(sizeof(struct backup));

   if (bck == NULL)
   {
      goto error;
   }

   memset(bck, 0, sizeof(struct backup));
   bck->valid = VALID_UNKNOWN;

   if (file != NULL)
   {
      while ((fgets(&buffer[0], sizeof(buffer), file)) != NULL)
      {
         char key[INFO_BUFFER_SIZE];
         char value[INFO_BUFFER_SIZE];
         char* ptr = NULL;

         memset(&key[0], 0, sizeof(key));
         memset(&value[0], 0, sizeof(value));

         ptr = strtok(&buffer[0], "=");

         if (ptr == NULL)
         {
            goto error;
         }

         memcpy(&key[0], ptr, strlen(ptr));

         ptr = strtok(NULL, "=");

         if (ptr == NULL)
         {
            goto error;
         }

         memcpy(&value[0], ptr, strlen(ptr) - 1);

         if (!strcmp(INFO_STATUS, &key[0]))
         {
            if (!strcmp("1", &value[0]))
            {
               bck->valid = VALID_TRUE;
            }
            else
            {
               bck->valid = VALID_FALSE;
            }
         }
         else if (!strcmp(INFO_LABEL, &key[0]))
         {
            memcpy(&bck->label[0], &value[0], strlen(&value[0]));
         }
         else if (!strcmp(INFO_WAL, &key[0]))
         {
            memcpy(&bck->wal[0], &value[0], strlen(&value[0]));
         }
         else if (!strcmp(INFO_BACKUP, &key[0]))
         {
            bck->backup_size = strtoul(&value[0], &ptr, 10);
         }
         else if (!strcmp(INFO_RESTORE, &key[0]))
         {
            bck->restore_size = strtoul(&value[0], &ptr, 10);
         }
         else if (!strcmp(INFO_ELAPSED, &key[0]))
         {
            bck->elapsed_time = atoi(&value[0]);
         }
         else if (!strcmp(INFO_VERSION, &key[0]))
         {
            bck->version = atoi(&value[0]);
         }
         else if (!strcmp(INFO_MINOR_VERSION, &key[0]))
         {
            bck->minor_version = atoi(&value[0]);
         }
         else if (!strcmp(INFO_KEEP, &key[0]))
         {
            bck->keep = atoi(&value[0]) == 1 ? true : false;
         }
         else if (!strcmp(INFO_TABLESPACES, &key[0]))
         {
            bck->number_of_tablespaces = strtoul(&value[0], &ptr, 10);
         }
         else if (pgmoneta_starts_with(&key[0], "TABLESPACE_OID"))
         {
            memcpy(&bck->tablespaces_oids[tbl_idx], &value[0], strlen(&value[0]));
         }
         else if (pgmoneta_starts_with(&key[0], "TABLESPACE_PATH"))
         {
            memcpy(&bck->tablespaces_paths[tbl_idx], &value[0], strlen(&value[0]));
            /* This one is last */
            tbl_idx++;
         }
         else if (pgmoneta_starts_with(&key[0], "TABLESPACE"))
         {
            memcpy(&bck->tablespaces[tbl_idx], &value[0], strlen(&value[0]));
         }
         else if (pgmoneta_starts_with(&key[0], INFO_START_WALPOS))
         {
            sscanf(&value[0], "%X/%X", &bck->start_lsn_hi32, &bck->start_lsn_lo32);
         }
         else if (pgmoneta_starts_with(&key[0], INFO_END_WALPOS))
         {
            sscanf(&value[0], "%X/%X", &bck->end_lsn_hi32, &bck->end_lsn_lo32);
         }
         else if (pgmoneta_starts_with(&key[0], INFO_CHKPT_WALPOS))
         {
            sscanf(&value[0], "%X/%X", &bck->checkpoint_lsn_hi32, &bck->checkpoint_lsn_lo32);
         }
         else if (pgmoneta_starts_with(&key[0], INFO_START_TIMELINE))
         {
            bck->start_timeline = atoi(&value[0]);
         }
         else if (pgmoneta_starts_with(&key[0], INFO_END_TIMELINE))
         {
            bck->end_timeline = atoi(&value[0]);
         }
         else if (pgmoneta_starts_with(&key[0], INFO_HASH_ALGORITHM))
         {
            bck->hash_algoritm = atoi(&value[0]);
         }
         else if (pgmoneta_starts_with(&key[0], INFO_COMMENTS))
         {
            memcpy(&bck->comments[0], &value[0], strlen(&value[0]));
         }
         else if (pgmoneta_starts_with(&key[0], INFO_EXTRA))
         {
            memcpy(&bck->comments[0], &value[0], strlen(&value[0]));
         }
      }
   }

   *backup = bck;

   if (file != NULL)
   {
      fclose(file);
   }

   return 0;

error:

   free(bck);

   if (file != NULL)
   {
      fclose(file);
   }

   return 1;
}

int
pgmoneta_get_number_of_valid_backups(int server)
{
   char* server_path = NULL;
   int number_of_backups = 0;
   struct backup** backups = NULL;
   int result = 0;

   server_path = pgmoneta_get_server_backup(server);
   if (server_path == NULL)
   {
      goto error;
   }

   if (pgmoneta_get_backups(server_path, &number_of_backups, &backups))
   {
      goto error;
   }

   for (int i = 0; i < number_of_backups; i++)
   {
      if (backups[i] != NULL && backups[i]->valid)
      {
         result++;
      }
   }

   for (int i = 0; i < number_of_backups; i++)
   {
      free(backups[i]);
   }
   free(backups);

   free(server_path);

   return result;

error:

   return 0;
}
