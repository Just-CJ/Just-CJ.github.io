interface TagItemProps {
  tag: string;
  count: number;
  colorIndex: number;
}

export function TagItem({ tag, count }: TagItemProps) {
  return (
    <a
      href={`/tags/${tag.toLowerCase().replace(/\//g, '-')}`}
      aria-label={`查看标签「${tag}」的 ${count} 篇文章`}
      className="journal-tag"
    >
      <span>{tag}</span>
      <span>{count}</span>
    </a>
  );
}
