import { cn } from '@lib/utils';
import type { ReactNode } from 'react';

export default function Divider({ children, className }: { children?: ReactNode; className?: string }) {
  return (
    <div className={cn('journal-section-title', className)}>
      {children ? <h2>{children}</h2> : null}
      <span aria-hidden="true" />
    </div>
  );
}
